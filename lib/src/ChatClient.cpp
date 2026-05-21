#include <iostream>
#include "../include/ChatClient.h"
#include <netdb.h>      //address resolution functions
#include <sys/socket.h> //socket functions
#include <unistd.h>

ChatClient::ChatClient(std::string PORT, std::string IP)
{
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = getaddrinfo(IP.c_str(), PORT.c_str(), &hints, &res);
    if (s != 0)
    {
        // handle error in assigning addresses
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s)); // stderr -> descriptor 2
        exit(1);
    }

    sockfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        exit(1);
    }

    // 2. connect
    if (::connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        exit(1);
    }

    freeaddrinfo(res);

    FD_ZERO(&master);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);
    fd_max = sockfd;
}

// user must put this inside a loop
Event ChatClient::select_username()
{
    Event result{};
    std::string name;
    getline(std::cin, name);
    if (::send(sockfd, name.c_str(), name.size(), 0) == -1)
    {
        perror("send");
        exit(1);
    }
    char buffer[50];
    int bytes = ::recv(sockfd, buffer, sizeof(buffer), 0);
    buffer[bytes] = '\0';
    std::string temp(buffer);
    temp = temp.substr(1);
    result.message = temp;

    if (buffer[0] == 'E')
    {
        result.type = ERROR;
    }
    else
    {
        result.type = USER_JOIN;
    }

    return result;
}

//user must keep this inside loop
Event ChatClient::run()
{
    read_fds = master;
    Event result{};
    result.type = NONE;
    if (::select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1)
    {
        perror("select");
        exit(1);
    }

    for (int i = 0; i <= fd_max; i++)
    {
        if (FD_ISSET(i, &read_fds))
        {
            if (i == 0)
            {
                // client has data to send as 0 is triggered meaning stdin
                std::string msg;
                getline(std::cin, msg);
                if (::send(sockfd, msg.c_str(), msg.size(), 0) == -1)
                {
                    perror("send");
                }
            }
            else if (sockfd == i)
            {
                // server/other client has sent data
                char buffer[1024];
                int bytes = ::recv(sockfd, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0)
                {
                    result.type = FATAL;
                    result.message = "Server disconnected";
                    return result;
                }
                buffer[bytes] = '\0';
                std::string msg(buffer);
                
                if(msg[0] == 'S'){
                    result.type= SERVER;
                    result.message = msg.substr(1);
                }else if(msg[0] == 'M'){
                    result.type = MESSAGE;
                    result.message = msg.substr(1);
                }else if(msg[0] == 'J'){
                    result.type = USER_JOIN;
                    result.message = msg.substr(1);
                }else if(msg[0] == 'L'){
                    result.type = USER_LEFT;
                    result.message = msg.substr(1);
                }else if(msg[0] == 'P'){
                    result.type = PERSONAL;
                    result.message = msg.substr(1);
                }
                return result;
            }
            
        }

    }
    return result;
}