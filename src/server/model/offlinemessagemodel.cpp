#include "../../../include/server/model/offlinemessagemodel.hpp"
#include "../../../include/server/mysql/mysql.hpp"

#include <muduo/base/Logging.h>

//插入离线消息
bool OfflineMessageModel::insert(int userid , string message){
    MySQL mysql;
    string sql = "insert into offlinemessage values(" + to_string(userid) + ",'" +message + "')";
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "insert success";
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

//查询离线消息
vector<string> OfflineMessageModel::query(int userId){
    vector<string> messages;
    string sql = "select message from offlinemessage where userid=" + to_string(userId);
    MySQL mysql;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res))!= nullptr){
                messages.push_back(row[0]);   
            }
            mysql_free_result(res);
            return messages;
        }
        else{
            LOG_INFO << "query failed";
            return messages;
        }
    }
    else{
        LOG_INFO << "connect failed";
        return messages;
    }
}

//删除离线消息
void OfflineMessageModel::remove(int userId){
    string sql = "delete from offlinemessage where userid=" + to_string(userId);
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            LOG_INFO << "remove success";
        }
        else{
            LOG_INFO << "remove failed";
        }
    }
    else{
        LOG_INFO << "connect failed";
    }
}