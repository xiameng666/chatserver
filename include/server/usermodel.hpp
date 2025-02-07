#ifndef USERMODEL_H
#define USERMODEL_H

#include "user.hpp"

// User�������ݲ�����
class UserModel {
public:
    // �����û�����
    bool insert(User &user);

    // �����û�id��ѯ�û���Ϣ
    User query(int id);

    // �����û���״̬��Ϣ
    bool updateState(User &user);

    // �����û���״̬��Ϣ
    bool resetState();
};

#endif
