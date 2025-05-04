#include "chatserver.hpp"
#include <iostream>

using namespace std;
using namespace muduo::net;
using namespace muduo;

int main(){
    EventLoop loop;
    InetAddress addr("127.0.0.1",8080);
    ChatServer server(&loop,addr,"ChatServer");

    server.start();
    loop.loop();
}