#include<iostream>
#include<./ChatServer.h>
#include <netdb.h> //address resolution functions
#include <sys/socket.h> //socket functions


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

    if (::bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
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