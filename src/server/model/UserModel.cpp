#include "../../../include/server/model/UserModel.hpp"
#include "../../../include/server/mysql/mysql.hpp"

#include <iostream>
using namespace std;

#include <muduo/base/Logging.h>

//user表的增加方法
bool UserModel::insert(User &user){
    //组装sql语句
    string sql = "insert into user(name,password,state) values('" + user.getName() + "','" + user.getPassword() + "','" + user.getState() + "')";
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "insert success";
            //获取插入成功的的用户在user表中的主键的值
            user.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
        else{
            LOG_INFO << "insert failed";
            return false;
        }
    }
    else{
        LOG_INFO << "connect failed";
        return false;
    }

}



//根据用户id查询用户信息
User UserModel::query(int id){
    //组装sql语句
    string sql = "select * from user where id=" + to_string(id);
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr){ 
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPassword(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
            else{
                LOG_INFO << "query failed";
                return User();
            }
        }
        else{
            LOG_INFO << "query failed";
            return User();
        }
        
    }
    else{
        LOG_INFO << "connect failed";
        return false;
    }
}

 //更新用户状态信息
bool UserModel::updateState(User user){
    //组装sql语句
    string sql = "update user set state='" + user.getState() + "' where id=" + to_string(user.getId());
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "update success";
            return true;
        }
        else{
            LOG_INFO << "update failed";
            return false;
        }
    }
    else{
        LOG_INFO << "connect failed";
        return false;
    }
}

//重置用户状态信息
void UserModel::resetState(){
    //组装sql语句
    string sql = "update user set state='offline' where state='online'";
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "reset success";
        }
        else{
            LOG_INFO << "reset failed";
        }
    }
}