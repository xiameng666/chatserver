#include "chatservice.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include <user.hpp>

using namespace muduo;

ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

ChatService::ChatService()
{
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
}

MsgHanlder ChatService::getHandler(int msgid)
{
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认处理器 空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp)
        {
            LOG_ERROR << "msgd:" << msgid << "can not find handler";
        };
    }
    else
    {
        return _msgHandlerMap[msgid];
    }
}

void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    try
    {
        int id = js["id"].get<int>();
        string pwd = js["password"];

        User user = _userModel.query(id);
        if (user.getId() == id && user.getPwd() == pwd)
        {
            if (user.getState() == "online")
            {
                json response;
                response["msgid"] = LOGIN_MSG_ACK;
                response["errno"] = 2;
                response["msg"] = "该用户已在线，请重新输入账号";
                conn->send(response.dump());
            }
            else
            {
                {
                    // 登陆成功，更新用户连接 、 状态信息
                    lock_guard<mutex> lock(_connMutex);
                    _userConnMap.insert({id, conn});
                }

                user.setState("online");
                _userModel.updateState(user);

                json response;
                response["msgid"] = LOGIN_MSG_ACK;
                response["errno"] = 0;
                response["id"] = user.getId();
                response["name"] = user.getName();
                conn->send(response.dump());
            }
        }
        else
        {
            // 用户不存在、用户存在但密码错误
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["msg"] = "用户名或密码错误";
            response["id"] = user.getId();
            response["name"] = user.getName();

            conn->send(response.dump());
        }
    }
    catch (const json::exception &e)
    {
        // 处理 JSON 解析异常
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 4;
        response["msg"] = string(e.what());
        conn->send(response.dump());
        return;
    }
}

void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user);
    if (state)
    {
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        json response;
        // 注册失败
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump());
    }
}

void ChatService::onChat(const TcpConnectionPtr &conn, json &js, Timestamp)
{
    int toid = js["toid"].get<int>();

    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end())
        {
            //toid在线，转发消息
            it->second->send(js.dump());
            return;
        }
    }
    
    //toid不在线，存储离线消息 TODO 

}

void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        lock_guard<mutex> lock(_connMutex);
        for (auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                //从map删除user连接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    if(user.getId()!=-1)
    {
        //更新用户状态信息
        user.setState("offline");
        _userModel.updateState(user);
    }
}