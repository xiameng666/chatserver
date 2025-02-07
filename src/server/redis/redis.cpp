
#include <iostream>
#include <thread>
#include "redis.hpp"

Redis::Redis()
    : _publish_context(nullptr), _subscribe_context(nullptr)
{
}

Redis::~Redis()
{
    if (_publish_context != nullptr)
    {
        redisFree(_publish_context);
    }

    if (_subscribe_context != nullptr)
    {
        redisFree(_subscribe_context);
    }
}

bool Redis::connect()
{
    // 负责发布消息的上下文连接
    _publish_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _publish_context)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    // 负责订阅消息的上下文连接
    _subscribe_context = redisConnect("127.0.0.1", 6379);
    if (nullptr == _subscribe_context)
    {
        cerr << "connect redis failed!" << endl;
        return false;
    }

    // 在单独的线程中，监听通道上的事件
    thread t([&]() {
        observer_channel_message();
    });
    t.detach();

    cout << "connect redis success!" << endl;
    return true;
}

bool Redis::publish(int channel, string message)
{
    redisReply *reply = (redisReply *)redisCommand(_publish_context, "PUBLISH %d %s", channel, message.c_str());
    if (nullptr == reply)
    {
        cerr << "publish command failed!" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}

bool Redis::subscribe(int channel)
{
    // SUBSCRIBE命令本身会造成线程阻塞等待通道里面发生消息，这里只做订阅通道，不接收通道消息
    // 通道消息的接收专门在observer_channel_message函数中的独立线程中进行
    // 只负责发送命令，不阻塞接收redis server响应消息，否则和notifyMsg线程抢占响应资源
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "SUBSCRIBE %d", channel))
    {
        cerr << "subscribe command failed!" << endl;
        return false;
    }
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            cerr << "subscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}

bool Redis::unsubscribe(int channel)
{
    if (REDIS_ERR == redisAppendCommand(this->_subscribe_context, "UNSUBSCRIBE %d", channel))
    {
        cerr << "unsubscribe command failed!" << endl;
        return false;
    }
    // redisBufferWrite可以循环发送缓冲区，直到缓冲区数据发送完毕（done被置为1）
    int done = 0;
    while (!done)
    {
        if (REDIS_ERR == redisBufferWrite(this->_subscribe_context, &done))
        {
            cerr << "unsubscribe command failed!" << endl;
            return false;
        }
    }
    return true;
}

void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;
    while (REDIS_OK == redisGetReply(this->_subscribe_context, (void **)&reply))
    {
        // 订阅收到的消息是一个带三元素的数组
        if (reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            // 给业务层上报通道上发生的消息
            _notify_message_handler(atoi(reply->element[1]->str) , reply->element[2]->str);
        }

        freeReplyObject(reply);
    }

    cerr << ">>>>>>>>>>>>> observer_channel_message quit <<<<<<<<<<<<<" << endl;
}

void Redis::init_notify_handler(function<void(int, string)> fn)
{
    this->_notify_message_handler = fn;
}

bool Redis::setOfflineMessage(int userid, string msg) {
    string key = "offline_msg:" + to_string(userid);
    lock_guard<mutex> lock(_mutex);
    
    redisReply *reply = (redisReply*)redisCommand(
        _context, "LPUSH %s %s", key.c_str(), msg.c_str());
    
    if (reply == nullptr) {
        return false;
    }
    
    freeReplyObject(reply);
    return true;
}

vector<string> Redis::getOfflineMessage(int userid) {
    string key = "offline_msg:" + to_string(userid);
    vector<string> messages;
    lock_guard<mutex> lock(_mutex);
    
    redisReply *reply = (redisReply*)redisCommand(
        _context, "LRANGE %s 0 -1", key.c_str());
    
    if (reply == nullptr) {
        return messages;
    }
    
    // 获取所有消息
    for (size_t i = 0; i < reply->elements; ++i) {
        messages.push_back(reply->element[i]->str);
    }
    
    freeReplyObject(reply);
    return messages;
}

bool Redis::deleteOfflineMessage(int userid) {
    string key = "offline_msg:" + to_string(userid);
    lock_guard<mutex> lock(_mutex);
    
    redisReply *reply = (redisReply*)redisCommand(
        _context, "DEL %s", key.c_str());
    
    if (reply == nullptr) {
        return false;
    }
    
    freeReplyObject(reply);
    return true;
}

bool Redis::setUserState(int userid, string state) {
    string key = "user_state:" + to_string(userid);
    lock_guard<mutex> lock(_mutex);
    
    redisReply *reply = (redisReply*)redisCommand(
        _context, "SET %s %s", key.c_str(), state.c_str());
    
    if (reply == nullptr) {
        return false;
    }
    
    freeReplyObject(reply);
    return true;
}

string Redis::getUserState(int userid) {
    string key = "user_state:" + to_string(userid);
    lock_guard<mutex> lock(_mutex);
    
    redisReply *reply = (redisReply*)redisCommand(
        _context, "GET %s", key.c_str());
    
    if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
        if (reply) freeReplyObject(reply);
        return "offline";
    }
    
    string state = reply->str;
    freeReplyObject(reply);
    return state;
} 