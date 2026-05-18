#include<iostream>
#include<./ChatServer.h>
#include <netdb.h> //address resolution functions
#include <sys/socket.h> //socket functions
#include <unistd.h>


ChatServer::ChatServer(std::string port, int clients){
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints)); //initialize hints with 0s

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = getaddrinfo(NULL, port.c_str(), &hints, &res);
    if(status != 0){
        // handle error in assigning addresses
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status)); // stderr -> descriptor 2
        exit(1);
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd == -1){
        perror("socket");
        exit(1);
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    { //:: makes it use global namespace not std
        perror("bind");
        exit(1);
    }

    if(listen(sockfd, clients) == -1){
        perror("listen");
        exit(1);
    }

    freeaddrinfo(res); //res gets dynamically allocated inside getaddrinfo, so we free it manually

    FD_ZERO(&master);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);

    fd_max = sockfd;
    //fd sets are initialized, listning socket is ready.
}


Event ChatServer::run(){
    //start with select
    struct Event result{};
    read_fds = master;

    if(select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){ //block till event occurs
        perror("select");
        return result;
    }

    //check which event and update Event accordingly
    for(int i=0 ; i<=fd_max ; i++){
        if(FD_ISSET(i, &read_fds)){
            if(i == sockfd){
                //received new connection
                result = handle_new_connections();
            }else if(i == 0){
                result = send_message();
            }else{
                result = handle_received_message(i);
            }
            return result;
        }
    }

    return result;

}


Event ChatServer::handle_new_connections()
{

    struct Event result{};
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    addr_size = sizeof(their_addr);

    int newfd = ::accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (newfd == -1)
    {
        perror("accpet");
        result.type = ERROR;
        result.message = "accept error";
        return result;
    }

    // add connection to fd set
    FD_SET(newfd, &master);
    if (newfd > fd_max)
    {
        fd_max = newfd;
    }

    result.type = NONE;
    return result;

}



Event ChatServer::send_message(){
    std::string msg;
    std::cin.clear();
    std::getline(std::cin, msg);
    struct Event result{};
    result.type = SERVER;
    msg = 'S'+msg;
    result.message = msg;

    for (int j = 1; j <= fd_max; j++)
    {
        if (FD_ISSET(j, &master) && j != sockfd)
        {
            if (send(j, msg.c_str(), msg.size(), 0) == -1)
            {
                perror("send");
                continue;
            }
        }
    }

    return result;
}



Event ChatServer::handle_received_message(int i){
    int bytes;
    char buffer[1024];
    // 6. receive data from clients
    bytes = recv(i, buffer, sizeof(buffer) - 1, 0);
    Event result{};
    result.type=NONE;

    if (bytes <= 0)
    {
        //close connection
        if (usernames.find(i) != usernames.end())
        {
            std::string msg = "L"+usernames[i] + " left the chat ";
            result.type = USER_LEFT;
            result.message = msg;
            for (int j = 1; j <= fd_max; j++)
            {
                if (FD_ISSET(j, &master))
                {
                    if (j != sockfd && j != i)
                    {
                        if (send(j, msg.c_str(), msg.size(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                }
            }
        }
        FD_CLR(i, &master);
        close(i);
    }else
    {
        buffer[bytes] = '\0';

        // check if this is the first message, meaning this is the username
        if (usernames.find(i) == usernames.end())
        {
            std::string name = buffer;
            for(auto user:usernames){
                if(user.second == name){
                    std::string temp;
                    temp = "EUsername already taken";
                    if(::send(i, temp.c_str(), temp.size() , 0) == -1){
                        perror("send");
                    }
                    return result;
                }
            }

            usernames[i] = name;
            result.message = "J" + name + " joined the chat ";
            result.type = USER_JOIN;

            for (int j = 1; j <= fd_max; j++)
            {
                if (FD_ISSET(j, &master))
                {
                    if (j != sockfd)
                    {
                        if (::send(j, result.message.c_str(), result.message.size(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                }
            }

        }

        //handle commands
        else if(buffer[0] == '/'){
            return handle_commands(buffer, i);
        }else{

            // breoadcast the received message
            std::string full_msg = "M" +usernames[i] + ": " + buffer;
            result.message = full_msg;
            result.type = MESSAGE;
            for (int j = 1; j <= fd_max; j++)
            {
                if (FD_ISSET(j, &master))
                {
                    if (j != sockfd )
                    {
                        if (::send(j, full_msg.c_str(), full_msg.size(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                }
            }
        }
    }

    return result;
}

Event ChatServer::handle_commands(char* buffer,int i){
    Event result{};
    result.type = NONE;
    std::string msg = buffer;
    bool error = false;
    int pos = msg.find(' ');
    std::string cmd;
    if(pos == std::string::npos){
        cmd = msg.substr(1);
    }else{
        cmd = msg.substr(1, pos-1);
    }

    std::string ans = "";
    if(cmd == "users"){
        for(auto user:usernames){
            ans += user.second + "\n";
        }
    }else if(cmd == "rename"){
        int next = msg.find(' ', pos+1);
        std::string arg;
        if(next == std::string::npos){
            arg = msg.substr(pos+1);
        }else{
            arg = msg.substr(pos+1, next-pos);
        }

        if(arg.size() > 10){
            ans = "username too long, length must be less than 11\n";
            error = true;
        }else{
            for(auto user:usernames){
                if(user.second == arg){
                    ans = "Username already taken, try something else!\n";
                    error = true;
                }
            }
            if(!error){
                usernames[i] = arg;
                ans = "Username changed successfully!\n";
            }
        }

    }else if(cmd == "msg"){
        // private message 
        int next = msg.find(' ', pos+1);
        std::string name = msg.substr(pos+1, next-pos-1);
        if(next != std::string::npos && next+1<msg.size()){
            std::string text = msg.substr(next+1);
            int sock = -1;
            for(auto user:usernames){
                if(user.second == name){
                    sock = user.first;
                    break;
                }
            }
            if(sock == -1){
                error = true;
                ans = "Username not found\n";
            }else{
                std::string privat = "";
                privat = "P" + usernames[i] + ": " + text + "\n";
                privat = privat;
                if(send(sock, privat.c_str(), privat.size(), 0)==-1){
                    perror("send");
                }
                ans = "Message sent successfully!\n";

            }

        }else{
            error = true;
            ans = "Add some message to send\n";
        }
        

    }else{
        ans = "Invalid command\n";
        error = true;
    }

    if(error){
        ans = "E"+ans;
    }else{
        ans = "S"+ans;
    }

    if(::send(i, ans.c_str(), ans.size(), 0) == -1){
        perror("send");
    }
    
    //no need to return anything valuable to server here
    return result;

}
