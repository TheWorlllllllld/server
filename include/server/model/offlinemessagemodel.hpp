#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H

#include <string>
#include <vector>
using namespace std;

class OfflineMessageModel
{
public:
    //插入离线消息
    bool insert(int userid , string message);
    
    //查询离线消息
    vector<string> query(int userId);
    
    //删除离线消息
    void remove(int userId);
};






#endif