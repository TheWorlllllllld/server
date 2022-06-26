#include "../../../include/server/model/groupmodel.hpp"
#include "../../../include/server/mysql/mysql.hpp"

//创建群组
bool GroupModel::createGroup(Group &group){
    string sql = "insert into allgroup(groupname,groupdesc) values('" + group.getName() + "','" + group.getDesc() + "')";
    MySQL mysql;
    if(mysql.connect()){
        if(mysql.update(sql)){
            //插入成功
            group.setId(mysql_insert_id(mysql.getConnection()));
            return true;
        }
    }
    return false;
}
//加入群组
void GroupModel::addGroup(int userid,int groupid,string role){
    string sql = "insert into groupuser values(" + to_string(groupid) + "," + to_string(userid) + ",'" + role + "')";
    MySQL mysql;
    if(mysql.connect()){
        mysql.update(sql);
    }
}
//查询用户所在群组信息
vector<Group> GroupModel::queryGroups(int userid){
    string sql  = "select a.id,a.groupname,a.groupdesc from allgroup a inner join groupuser b where a.id = b.groupid and b.userid = " + to_string(userid);
    MySQL mysql;
    vector<Group> groupVec;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                Group group;
                group.setId(atoi(row[0]));
                group.setName(row[1]);
                group.setDesc(row[2]);
                groupVec.push_back(group);
            }
            mysql_free_result(res);
        }
    }

    //查询用户群组的用户信息
    for(Group &group : groupVec){
        sql = "select a.id,a.name,a.state,b.grouprole from user a inner join groupuser b on b.userid = a.id  where b.groupid = " + to_string(group.getId());
        MYSQL_RES *res = mysql.query(sql);
        if(res != nullptr){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                GroupUser groupUser;
                groupUser.setId(atoi(row[0]));
                groupUser.setName(row[1]);
                groupUser.setState(row[2]);
                groupUser.setRole(row[3]);
                group.getUsers().push_back(groupUser);
            }
            mysql_free_result(res);
        }
    }
    return groupVec;
}
//根据groupid查询群组用户ld列表，除userid自己
vector<int> GroupModel::queryGroupUsers(int userid,int groupid){
    string sql = "select userid from groupuser where groupid = " + to_string(groupid) + " and userid != " + to_string(userid);
    MySQL mysql;
    vector<int> idVec;
    if(mysql.connect()){
        MYSQL_RES *res = mysql.query(sql);
        if(res){
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr){
                idVec.push_back(atoi(row[0]));
            }
            mysql_free_result(res);
        }
    }
    return idVec;
}