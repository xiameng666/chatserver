#ifndef  CHATSERVICE_H
#define  CHATSERVICE_H

#include<muduo/net/TcpConnection.h>
#include<unordered_map>
#include<functional>
#include<mutex>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "json.hpp"
using json  = nlohmann::json;

#include "usermodel.hpp"

using MsgHanlder = std::function<void(const TcpConnectionPtr &conn,json &js,Timestamp)>;

class ChatService
{
public:
    //获取单例
    static ChatService* instance();

    //登录
    void login(const TcpConnectionPtr &conn,json &js,Timestamp);

    //注册
    void reg(const TcpConnectionPtr &conn,json &js,Timestamp);

    //一对一聊天
    void onChat(const TcpConnectionPtr &conn,json &js,Timestamp);

    //获取消息对应的处理器
    MsgHanlder getHandler(int msgid);

    //客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
private:
    ChatService();

    //存储消息id对应业务的map
    unordered_map<int,MsgHanlder> _msgHandlerMap;

    //存储在线用户的通信连接信息
    unordered_map<int,TcpConnectionPtr> _userConnMap;

    //_userConnMap线程安全
    mutex _connMutex;

    UserModel _userModel;

};

#endif