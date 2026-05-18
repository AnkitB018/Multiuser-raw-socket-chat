#include<string>
#include<map>
#include <sys/select.h> //fd sets

#pragma once //makes sure header is included only once

enum EventType{
    MESSAGE,
    USER_JOIN,
    USER_LEFT,
    ERROR
};

struct Event{
    EventType type;
    std::string message;
};

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
        ChatServer(std::string port, int clients = 10);
        Event run();
        Event handle_new_connections();
        Event send_message();
        Event handle_received_message(int i);
        Event handle_commands(char *buffer, int i);
};