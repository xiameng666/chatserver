#include "db.h"

#include <muduo/base/Logging.h>
using namespace muduo;

// ���ݿ�������Ϣ
static string server = "127.0.0.1";
static string user = "root";
static string password = "123456";
static string dbname = "chat_system";


// ��ʼ�����ݿ�����
MySQL::MySQL() {
    _conn = mysql_init(nullptr);
}

// �ͷ����ݿ�������Դ
MySQL::~MySQL() {
    if (_conn != nullptr)
        mysql_close(_conn);
}

// �������ݿ�
bool MySQL::connect() {
    MYSQL* p = mysql_real_connect(_conn, server.c_str(), 
        user.c_str(), password.c_str(), 
        dbname.c_str(), 3306, nullptr, 0);
    if (p != nullptr) {
        // �޸�Ϊ UTF-8 ����
        mysql_query(_conn, "set names utf8");
        LOG_INFO << "connect mysql success!";
        return true;
    }
    // ��Ӵ�����Ϣ���
    LOG_ERROR << "connect mysql fail: " << mysql_error(_conn);
    return false;
}

// ���²���
bool MySQL::update(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                << sql << " ����ʧ�ܣ�";
        return false;
    }
    return true;
}

// ��ѯ����
MYSQL_RES* MySQL::query(string sql) {
    if (mysql_query(_conn, sql.c_str())) {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                << sql << " ��ѯʧ�ܣ�";
        return nullptr;
    }
    return mysql_use_result(_conn);
};

// ��ȡ����
MYSQL* MySQL::getConnection() {
    return _conn;
}
