#include<string>
#include<map>
#include"./ChatServer.h"

#pragma once //makes sure header is included only once


class ChatClient{
    private:
        int sockfd;
        fd_set master, read_fds;
        int fd_max;
        
    public:
        ChatClient(std::string PORT, std::string IP);
        Event select_username();
        Event run();
};