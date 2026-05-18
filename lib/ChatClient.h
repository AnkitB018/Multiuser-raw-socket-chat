#include<string>
#include<map>
#include<./ChatServer.h>

#pragma once //makes sure header is included only once


class ChatClient{
    private:
        int sockfd;
    public:
        ChatClient(int PORT, std::string IP);
        Event run();
};