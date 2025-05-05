#include "chatserver.hpp"
#include "json.hpp"
#include "chatservice.hpp"
#include <iostream>
#include<functional>
using std::string;
using std::bind;
using namespace muduo;
using namespace muduo::net;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

using json=nlohmann::json;

ChatServer::ChatServer(EventLoop *loop,
                        const InetAddress &listenAddr,
                        const string &nameArg)
    :_server(loop,listenAddr,nameArg),_loop(loop)
{
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,_1));
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,_1,_2,_3));
    _server.setThreadNum(4);
}
void ChatServer::start()
{
    _server.start();
}

void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn,
                Buffer* buffer,
                Timestamp time)
{
    string buf=buffer->retrieveAllAsString();

    // 测试，添加json打印代码
    std::cout << buf << std::endl;

    json js=json::parse(buf);
    //解耦网络模块代码和业务代码模块

    auto msgHandler=ChatService::instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn,js,time);
}   