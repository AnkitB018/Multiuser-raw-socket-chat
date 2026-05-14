#include <iostream> // cout, cin
#include <string>   // std::string
#include <vector>   // std::vector
#include <cstring>  // memset

#include <unistd.h>     // close
#include <arpa/inet.h>  // sockaddr_in, inet_*
#include <sys/socket.h> // socket, bind, listen, accept, recv, send
#include <netdb.h>      // getaddrinfo, freeaddrinfo
#include <cstdio>
#include <cstdlib>
#include <map>
#include<ctime>



using namespace std;



#define GREEN  "\033[32m"
#define BLUE   "\033[34m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define MAGENTA "\033[35m"
#define RESET  "\033[0m"
#define RED "\033[31m"


#define PORT "3456"
#define backlog 10


string get_time() 
{
    time_t now = time(0);
    tm *ltm = localtime(&now);

    char buf[10];
    snprintf(buf, sizeof(buf), "%02d:%02d", ltm->tm_hour, ltm->tm_min);
    return string(buf);
}


void handle_commands(char *buffer, int i, map<int, string>&usernames)
{
    string msg = buffer;
    bool error = false;
    int pos = msg.find(' ');
    string cmd;
    if(pos == string::npos){
        cmd = msg.substr(1);
    }else{
        cmd = msg.substr(1, pos-1);
    }

    string ans = "";
    if(cmd == "users"){
        for(auto user:usernames){
            ans += user.second + "\n";
        }
    }else if(cmd == "rename"){
        int next = msg.find(' ', pos+1);
        string arg;
        if(next == string::npos){
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
        string name = msg.substr(pos+1, next-pos-1);
        if(next != string::npos && next+1<msg.size()){
            string text = msg.substr(next+1);
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
                string privat = "";
                privat = "[" + get_time() + "] "+ string("Private Message-> ") + usernames[i] + ": " + text + "\n";
                privat = YELLOW + privat + RESET;
                if(::send(sock, privat.c_str(), privat.size(), 0)==-1){
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
        ans = RED+ans+RESET;
    }else{
        ans = MAGENTA + ans + RESET;
    }

    if(::send(i, ans.c_str(), ans.size(), 0) == -1){
        perror("send");
    }


}


void handle_new_connections(struct sockaddr_storage &their_addr, socklen_t &addr_size, int &sockfd, fd_set &master, int &fd_max)
{

    addr_size = sizeof(their_addr);
    // 4. accept new connection
    int newfd = ::accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
    if (newfd == -1)
    {
        perror("accpet");
        return;
    }

    // add connection to fd set
    FD_SET(newfd, &master);
    if (newfd > fd_max)
    {
        fd_max = newfd;
    }
}


void send_message(int &fd_max, int &i, int &sockfd, fd_set &master)
{
    string msg;
    cin.clear();
    getline(cin, msg);
    msg = "[" + get_time() + "] " +"Server: " + msg;
    msg = MAGENTA + msg + RESET;
    // 5. send data to all, broadcast the message basically
    for (int j = 1; j <= fd_max; j++)
    {
        if (FD_ISSET(j, &master) && j != sockfd)
        {
            if (::send(j, msg.c_str(), msg.size(), 0) == -1)
            {
                perror("send");
                continue;
            }
        }
    }
}


void handle_received_message(int &i, int &sockfd, map<int, string> &usernames, int &fd_max, fd_set &master)
{
    int bytes;
    char buffer[1024];
    // 6. receive data from clients
    bytes = ::recv(i, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0)
    {
        //close connection
        if (usernames.find(i) != usernames.end())
        {
            string msg = "[" + get_time() + "]" + " ------- " + usernames[i] + " has left the chat -------";
            msg = YELLOW + msg + RESET;
            cout << msg << endl;
            for (int j = 1; j <= fd_max; j++)
            {
                if (FD_ISSET(j, &master))
                {
                    if (j != sockfd && j != i)
                    {
                        if (::send(j, msg.c_str(), msg.size(), 0) == -1)
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
    }

    else
    {
        buffer[bytes] = '\0';

        // check if this is the first message, meaning this is the username
        if (usernames.find(i) == usernames.end())
        {
            string name = buffer;
            for(auto user:usernames){
                if(user.second == name){
                    string temp = "NO";
                    if(::send(i, temp.c_str(), temp.size() , 0) == -1){
                        perror("send");
                        return;
                    }
                    return;
                }
            }

            usernames[i] = name;
            string temp = "OK";
            if(::send(i, temp.c_str(), temp.size(), 0) == -1){
                perror("send");
            }

            name = "[" + get_time() + "]" + " ------- " + name + " has joined the chat -------";
            name = GREEN + name + RESET;

            cout<<name<<endl;
            for (int j = 1; j <= fd_max; j++)
            {
                if (FD_ISSET(j, &master))
                {
                    if (j != sockfd && j != i)
                    {
                        if (::send(j, name.c_str(), name.size(), 0) == -1)
                        {
                            perror("send");
                            continue;
                        }
                    }
                }
            }

            return;
        }

        //handle commands
        if(buffer[0] == '/'){
            handle_commands(buffer, i, usernames);
            return;
        }

        // breoadcast the received message
        string full_msg = "[" + get_time() + "] " +usernames[i] + ": " + buffer;
        full_msg = BLUE + full_msg + RESET;
        cout << full_msg << endl;
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




int main()
{
    cout << "--------------------------SERVER--------------------------" << endl;
    struct addrinfo hints, *res;
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    int newfd;
    fd_set master, read_fds;
    map<int, string> usernames;

    memset(&hints, 0, sizeof(hints)); // initialize all of hints with 0s

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int status = ::getaddrinfo(NULL, PORT, &hints, &res); // get addresses assigned from hints and now info in res
    if (status != 0)
    {
        // handle error
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status)); // stderr -> descriptor 2
        exit(1);
    }

    // 1. Get the socket fd
    int sockfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    // 2. Bind the socket with address
    if (::bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    { //:: makes it use global namespace not std
        perror("bind");
        exit(1);
    }

    // 3. start listning for new connections
    if (::listen(sockfd, backlog) == -1)
    {
        perror("listen");
        exit(1);
    }

    char ip[INET_ADDRSTRLEN];
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;

    inet_ntop(AF_INET, &(ipv4->sin_addr), ip, sizeof(ip));

    cout << CYAN <<"Server is listening on: " << ip << ":" << PORT << RESET <<endl;

    // free the res, no more use of it
    ::freeaddrinfo(res);

    FD_ZERO(&master);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);

    int fd_max = sockfd;
    while (true)
    {

        read_fds = master;
        // use select blocking call to monitor which fd is set
        if (::select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1)
        {
            perror("select");
            exit(1);
        }

        for (int i = 0; i <= fd_max; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {

                if (i == sockfd)
                {
                    // new connection arrives
                    handle_new_connections(their_addr, addr_size, i, master, fd_max);
                }
                else if (i == 0)
                {
                    // server wants to send data
                    send_message(fd_max, i, sockfd, master);
                }
                else
                {
                    // someone has sent data
                    handle_received_message(i, sockfd, usernames, fd_max, master);
                }
            }
        }
    }

    return 0;
}