#include <iostream>     // cout, cin
#include <string>       // std::string
#include <vector>       // std::vector
#include <cstring>      // memset

#include <unistd.h>     // close
#include <arpa/inet.h>  // sockaddr_in, inet_*
#include <sys/socket.h> // socket, bind, listen, accept, recv, send
#include <netdb.h>      // getaddrinfo, freeaddrinfo
#include <cstdio>
#include <cstdlib>


using namespace std;

#define GREEN  "\033[32m"
#define BLUE   "\033[34m"
#define YELLOW "\033[33m"
#define CYAN   "\033[36m"
#define MAGENTA "\033[35m"
#define RESET  "\033[0m"
#define RED "\033[31m"



int main(int argc, char* argv[]){
    string IP;
    string PORT = "3456";

    if(argc >= 2){
        IP = argv[1];
    }else{
        IP = "127.0.0.1";
    }
    if(argc >= 3){
        PORT = argv[2];
    }

    cout<<"---------------------CLIENT---------------------"<<endl;
    cout<<MAGENTA<<"Commands:\n\nAnything that start with a '/' is treated as command\n/users: Username of all connected users\n/rename name: change your current username to 'name'\n/msg user text: Use for private messaging, here text will be sent to user\n"<<RESET<<endl;
    struct addrinfo hints, *res;
    fd_set master, read_fds; 



    memset(&hints,0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int s = ::getaddrinfo(IP.c_str(), PORT.c_str(), &hints, &res);
    if(s != 0){
        fprintf(stderr, "get_addrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    //1. create socket fd
    int sockfd = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if(sockfd == -1){
        perror("socket");
        exit(1);
    }

    //2. connect
    if(::connect(sockfd, res->ai_addr, res->ai_addrlen) == -1){
        perror("connect");
        exit(1);
    }

    freeaddrinfo(res);

    //take username for use
    while(true){

        cout<<MAGENTA << ">> Enter USERNAME for Chat this session: "<< RESET;
        string name;
        getline(cin, name);
        if(::send(sockfd, name.c_str(), name.size() , 0) == -1){
            perror("send");
            exit(1);
        }
        char buffer[50];
        int bytes = ::recv(sockfd, buffer, sizeof(buffer), 0);
        buffer[bytes] = '\0';
        string temp(buffer);
        if(temp == "OK"){
            cout<<GREEN<<"Username granted!"<<RESET<<endl;
            break;
        }
        cout<<RED<<"Username already taken, try something else"<<RESET<<endl;

    }

    FD_ZERO(&master);
    FD_SET(sockfd, &master);
    FD_SET(0, &master);
    int fd_max = sockfd;

    while(true){
        read_fds = master;
        if(::select(fd_max+1, &read_fds, NULL, NULL, NULL) == -1){
            perror("select");
            exit(1);
        }

        for(int i=0 ; i<=fd_max ; i++){
            if(FD_ISSET(i, &read_fds)){
                if(i == 0){
                    //client has data to send as 0 is triggered meaning stdin
                    string msg;
                    getline(cin, msg);
                    //3. send message to server
                    if(::send(sockfd, msg.c_str(), msg.size(), 0) == -1){
                        perror("send");
                    }
                }else if(sockfd == i){
                    // server/other client has sent data
                    //4. receive data from server
                    char buffer[1024];
                    int bytes = ::recv(sockfd, buffer, sizeof(buffer)-1, 0);
                    if(bytes <= 0){
                        cout<<RED<<"Server Disconnected!"<< RESET;
                        exit(0);
                    }
                    buffer[bytes] = '\0';
                    cout<<buffer<<endl;
                }



            }
        }


    }


    close(sockfd);

    return 0;
}