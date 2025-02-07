#include "chatserver.hpp"
#include <string>
#include "json.hpp"
#include <iostream>
#include <chatservice.hpp>


using namespace std;
using json  = nlohmann::json;

ChatServer::ChatServer( EventLoop* loop,//事件循环
                        const InetAddress& listenAddr,//监听地址 IP+PORT
                        const string& nameArg)//服务器名字
            :_server(loop,listenAddr,nameArg),_loop(loop)
{
    //给服务器注册用户连接的创建和断开回调
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection,this,std::placeholders::_1));

    //给服务器注册用户读写事件回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
                
    //设置服务器线程数量 1个I/O线程 3个worker线程
    _server.setThreadNum(4);
}

void ChatServer::start()
{
    _server.start();
}

void  ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        ChatService::instance()->clientCloseException(conn);
    }

}

void  ChatServer::onMessage(const TcpConnectionPtr& conn,
                            Buffer *buffer,
                            Timestamp time)
{
    //缓冲区->string->json 反序列化
    string buf = buffer->retrieveAllAsString();
    json js = json::parse(buf);
    
    auto handler = ChatService::instance()->getHandler(js["msgid"].get<int>());
    handler(conn,js,time);
} 