#include "../../../include/server/model/friendmodel.hpp"
#include "../../../include/server/mysql/mysql.hpp"

#include <iostream>

#include <muduo/base/Logging.h>

//添加好友关系
void FreindModel::insert(int userId,int friendId){
    string sql = "insert into friend values("+to_string(userId)+","+to_string(friendId)+")";
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "insert success";
        }
        else{
            LOG_INFO << "insert failed";
        }
    }
    else{
        LOG_INFO << "connect failed";
    }
}
//删除好友关系
void FreindModel::remove(int userId,int friendId){

}
//返回用户好友列表
vector<User> FreindModel::query(int userId){
    // string sql = "select * from user where id in (select friendId from friend where userId="+to_string(userId)+")";
    string sql = "select a.id,a.name,a.state from user a inner join friend b on a.id=b.friendid where b.userid="+to_string(userId);
    MySQL mysql;
    vector<User> vec;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
            return vec;
        }
        else{
            LOG_INFO << "query failed";
            return vec;
        }
    }
    else{
        LOG_INFO << "connect failed";
        return vec;
    }
}