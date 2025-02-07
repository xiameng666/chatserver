#include "db.h"

#include <muduo/base/Logging.h>
using namespace muduo;

// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat_system";


// 初始化数据库连接
MySQL::MySQL() {
    _conn = mysql_init(nullptr);
}

// 释放数据库连接资源
MySQL::~MySQL() {
    if (_conn != nullptr)
        mysql_close(_conn);
}

// 连接数据库
bool MySQL::connect() {
    MYSQL* p = mysql_real_connect(_conn, server.c_str(), 
        user.c_str(), password.c_str(), 
        dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr) {
        // 修改为 UTF-8 编码
        mysql_query(_conn, "set names utf8");
        LOG_INFO << "connect mysql success!";
        return true;
    }
    // 添加错误信息输出
    LOG_ERROR << "connect mysql fail: " << mysql_error(_conn);
    return false;
}

// 更新操作
bool MySQL::update(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                << sql << " 更新失败！";
        return false;
    }
    return true;
}

// 查询操作
MYSQL_RES* MySQL::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                << sql << " 查询失败！";
        return nullptr;
    }
    return mysql_use_result(_conn);
};

// 获取连接
MYSQL* MySQL::getConnection() {
    return _conn;
}
