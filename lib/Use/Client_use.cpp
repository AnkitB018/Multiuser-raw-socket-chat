#include<iostream>
#include"../ChatClient.h"

using namespace std;

#define PORT "3456"
#define IP "127.0.0.1"

int main(){
    cout<<"---------------------------CLIENT---------------------------"<<endl;
    ChatClient clt(PORT, IP);


    while(true){
        Event result;
        cout<<MAGENTA<<"Select username for this session: "<<RESET;
        result = clt.select_username();
        cout<<endl;
        if(result.type == ERROR){
            cout<<result.message<<endl;
            continue;
        }
        cout<<GREEN<<"Username granted!"<<RESET<<endl;
        break;
    }

    while(true){
        Event state;
        state = clt.run();

        if(state.type == NONE){
            continue;
        }
        if(state.type == ERROR){
            cout<<RED<<"An error occured:"<<state.message<<RESET<<endl;
            continue;
        }
        if(state.type == FATAL){
            cout<<RED<<state.message<<RESET<<endl;
            exit(0);
        }
        if(state.type == USER_JOIN){
            cout<<GREEN<<"--------------"<<state.message<<"--------------"<<RESET<<endl;
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
        if(state.type == PERSONAL){
            cout<<CYAN<<"Private message>> "<<state.message<<RESET<<endl;
        }
        if(state.type == SERVER){
            cout<<MAGENTA<<"SYSTEM: \n"<<state.message<<RESET<<endl;
        }
        
    }


    return 0;
}