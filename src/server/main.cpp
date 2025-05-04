#include "chatserver.hpp"
#include <iostream>
#include<signal.h>
#include<chatservice.hpp>

void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

using namespace std;
using namespace muduo::net;
using namespace muduo;

int main(){
    
    signal(SIGINT,resetHandler);

    EventLoop loop;
    InetAddress addr("127.0.0.1",8080);
    ChatServer server(&loop,addr,"ChatServer");

    server.start();
    loop.loop();
}