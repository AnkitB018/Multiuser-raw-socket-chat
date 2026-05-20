#include<iostream>
#include"../ChatServer.h"
#include<string>

#define PORT "3456"
using namespace std;
int main(){
    cout<<"---------------------------SERVER---------------------------"<<endl;
    ChatServer srv(PORT);

    while(true){
        Event state;
        state = srv.run();

        if(state.type == ERROR){
            cout<<RED<<"An error occured:"<<state.message<<RESET<<endl; // to colour text in terminal, defined in server header file
            continue;
        }
        if(state.type == USER_JOIN){
            cout<<GREEN"--------------"<<state.message<<"--------------"<<RESET<<endl;
            continue;
        }
        if(state.type == USER_LEFT){
            cout<<YELLOW<<"--------------"<<state.message<<"--------------"<<RESET<<endl;
            continue;
        }
        if(state.type == MESSAGE){
            cout<<BLUE<<state.message<<RESET<<endl;
            continue;
        }
        if(state.type == NONE){
            continue;
        }
    }

    return 0;
}