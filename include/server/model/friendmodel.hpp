#ifndef FRIENDMODEL_H
#define FRIENDMODEL _H
#include<vector>
#include "user.hpp"
using namespace std;

class FreindModel{
public:
    //添加好友关系
    void insert(int userId,int friendId);
    //删除好友关系
    void remove(int userId,int friendId);
    //返回用户好友列表
    vector<User> query(int userId);
};

#endif