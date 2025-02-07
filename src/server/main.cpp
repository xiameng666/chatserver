#include "chatserver.hpp"
#include <iostream>
#include <string>

using namespace std;

void usage() {
    cout << "Usage: ./ChatServer [ip] [port]" << endl;
    cout << "Example: ./ChatServer 127.0.0.1 6000" << endl;
}

int main(int argc, char* argv[])
{
    // 检查命令行参数
    if (argc != 3) {
        usage();
        return 1;
    }

    // 获取 IP 和端口参数
    const char* ip = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));

    // 参数验证
    if (port == 0) {
        cerr << "Invalid port number" << endl;
        usage();
        return 1;
    }

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");
    
    cout << "Server starting at " << ip << ":" << port << endl;
    
    server.start();
    loop.loop();  // 阻塞等待新用户连接，已连接用户的读写...

    return 0;
}