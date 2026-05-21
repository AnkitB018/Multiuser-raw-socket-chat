#include<string>
#include<map>
#include <sys/select.h> //fd sets

#pragma once //makes sure header is included only once

#define GREEN  "\033[32m"
#define BLUE   "\033[34m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define MAGENTA "\033[35m"
#define RESET  "\033[0m"
#define RED "\033[31m"

enum EventType{
    NONE, //N
    FATAL, //F
    SERVER, //S
    MESSAGE, //M
    USER_JOIN, //J
    USER_LEFT, //L
    ERROR, //E
    PERSONAL //P
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
        Event handle_new_connections();
        Event send_message();
        Event handle_received_message(int i);
        Event handle_commands(char *buffer, int i);

    public:
        int clients;
        ChatServer(std::string port, int clients = 10);
        Event run();
        
};