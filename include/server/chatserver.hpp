#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
using namespace muduo;
using namespace muduo::net;


class ChatServer
{
public:
     ChatServer(EventLoop* loop,//事件循环
            const InetAddress& listenAddr,//监听地址 IP+PORT
            const string& nameArg);//服务器名字

    void start();
private:
    //两个回调
    void onConnection(const TcpConnectionPtr& conn);

    void onMessage(const TcpConnectionPtr& conn,
                    Buffer *buffer,
                    Timestamp time);

    TcpServer _server;//组合muduo库，实现服务器功能的类对象
    EventLoop* _loop;//指向事件循环对象的指针
};

#endif