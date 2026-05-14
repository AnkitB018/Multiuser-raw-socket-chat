#include<string>
#include<map>
#include <sys/select.h> //fd sets

#pragma once //makes sure header is included only once

class ChatServer{
    private:
        int sockfd;
        int newfd;
        std::string PORT;
        std::map<int, std::string>usernames;
        fd_set master, read_fds;
        int fd_max;

    public:
        int clients;
        ChatServer(std::string port);
        void run(int clients = 10);
};