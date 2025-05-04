#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

class ChatServer
{
public:
    ChatServer(muduo::net::EventLoop* loop,
               const muduo::net::InetAddress& listenAddr,
               const std::string& nameArg);

    void start();

private:
    void onConnection(const muduo::net::TcpConnectionPtr&);
    void onMessage(const muduo::net::TcpConnectionPtr&,
                   muduo::net::Buffer*,
                   muduo::Timestamp);

    muduo::net::TcpServer _server;
    muduo::net::EventLoop* _loop;  // 假设EventLoop在muduo::net中
};

#endif