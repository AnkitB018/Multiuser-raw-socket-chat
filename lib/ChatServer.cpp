#include<iostream>
#include<./ChatServer.h>
#include <netdb.h> //address resolution functions
#include <sys/socket.h> //socket functions

ChatServer::ChatServer(std::string port){
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

    if (::bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    { //:: makes it use global namespace not std
        perror("bind");
        exit(1);
    }

    freeaddrinfo(res); //res gets dynamically allocated inside getaddrinfo, so we free it manually

    FD_ZERO(&master);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);

    fd_max = sockfd;
    //fd sets are initialized, listning socket is ready.
}

void ChatServer::run(int clients = 10){
    //start with listen
}