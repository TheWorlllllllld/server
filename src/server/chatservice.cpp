#include "../../include/server/chatservice.hpp"
#include "../../include/public.hpp"
#include <muduo/base/Logging.h>
#include <vector>
#include <map>

#include <muduo/base/Logging.h>


ChatService* ChatService::Instance(){
    static ChatService _instance;
    return &_instance;
}

ChatService::ChatService(){
    _HandlerMap[LOGIN_MSG] = bind(&ChatService::login,this,_1,_2,_3);
    _HandlerMap[REG_MSG] = bind(&ChatService::registerUser,this,_1,_2,_3);
    _HandlerMap[ONE_CHAT_MSG] = bind(&ChatService::oneChat,this,_1,_2,_3);
    _HandlerMap[ADD_FRIEND_MSG] = bind(&ChatService::addFriend,this,_1,_2,_3);
    _HandlerMap[CREATE_GROUP_MSG] = bind(&ChatService::createGroup,this,_1,_2,_3);
    _HandlerMap[ADD_GROUP_MSG] = bind(&ChatService::addGroup,this,_1,_2,_3);
    _HandlerMap[GROUP_CHAT_MSG] = bind(&ChatService::groupChat,this,_1,_2,_3);
    _HandlerMap[LOGINOUT_MSG] = bind(&ChatService::loginout,this,_1,_2,_3);

    //连接redis
    if(!_redis.connect()){
        //设置上报消息的回调
        _redis.init_notify_handler(std::bind(&ChatService::handleRedidSubScribeMessage ,this,_1,_2));
    }
}



//登录业务
void ChatService::login(const TcpConnectionPtr &conn,json &js,Timestamp){
    //获取用户名和密码
    int id = js["id"].get<int>();
    string password = js["password"].get<string>();
    //查询数据库，判断id和密码是否正确
    User user = _userModel.query(id);
    if( user.getId() ==  id && user.getPassword() == password){
        if(user.getState() == "online"){
            //用户已经在线
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1;
            response["ermsg"] = "该账号已经在线，请重新输入账号";
            conn->send(response.dump());
        }
        else{
            //登陆成功，记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap[id] = conn;
            }

            //登录成功后，向redis订阅
            _redis.subscribe(id);

            //登陆成功，更新用户状态信息
            user.setState("online");
            _userModel.updateState(user);
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            //查询该用户是否有离线消息
            vector<string> messages = _offlineMessageModel.query(id);
            if(!messages.empty()){
                response["offlinemsg"] = messages;
                //清除离线消息
                _offlineMessageModel.remove(id);
            }
            //查询该用户的好友列表
            vector<User> friends = _friendModel.query(id);
            if(!friends.empty()){
                vector<string> vec2;
                for(User &user : friends){
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState(); 
                    vec2.push_back(js.dump());
                }
                response["friends"] = vec2;
            }

            conn->send(response.dump());
        }
    }
    else{

        //用户不存在
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["ermsg"] = "用户名或密码错误";
        conn->send(response.dump());
    }


}

//注册业务
void ChatService::registerUser(const TcpConnectionPtr &conn,json &js,Timestamp){
    //获取用户名和密码
    string username = js["username"];
    string password = js["password"];
    User user;
    user.setName(username);
    user.setPassword(password);
    bool state = _userModel.insert(user);
    if(state){
        //注册成功
        //把用户名和密码返回给客户端
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else{
        //注册失败
        //把错误码返回给客户端
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["ermsg"] = "用户名或密码错误";
        conn->send(response.dump());
    }
}

//处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn){
    User user;
    //删除用户连接信息
    {    
        lock_guard<mutex> lock(_connMutex);
        for(auto it = _userConnMap.begin();it != _userConnMap.end();++it){
            if(it->second == conn){
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }

    //下线，取消redis注册的通道
    _redis.unsubscribe(user.getId());

    //更新用户状态信息
    if(user.getId() != -1){
        user.setState("offline");
        _userModel.updateState(user);
        LOG_INFO << "用户" << user.getId() << "退出了   ";
    } 
}

//单聊业务 
void ChatService::oneChat(const TcpConnectionPtr &conn,json &js,Timestamp){
    // int fromId = js["fromid"].get<int>();
    int toId = js["toid"].get<int>();
    bool userState = false;
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toId);
        if(it != _userConnMap.end()){
            //发送消息  服务器主动推送消息给toid用户
            it->second->send(js.dump());
            return; 
        }
    }

    //查询toid是否在线
    User User = _userModel.query(toId);
    if(User.getState() == "online"){
        _redis.publish(toId,js.dump());
        return;
    }

    _offlineMessageModel.insert(toId,js.dump());
}

//添加好友
void ChatService::addFriend(const TcpConnectionPtr &conn,json &js,Timestamp){
    int friendid = js["friendid"].get<int>();
    int userid = js["id"].get<int>();
    //存储好友信息
    _friendModel.insert(userid,friendid);
}

//创建群
void ChatService::createGroup(const TcpConnectionPtr &conn,json &js,Timestamp){
    int userid = js["id"].get<int>();
    string groupname = js["groupname"].get<string>();
    string desc = js["desc"].get<string>();
    //存储群信息
    Group group(-1,groupname,desc);
    if(_groupModel.createGroup(group)){
        _groupModel.addGroup(userid,group.getId(),"creator");
    }

}

//加入群
void ChatService::addGroup(const TcpConnectionPtr &conn,json &js,Timestamp){
    int groupid = js["groupid"].get<int>();
    int userid = js["id"].get<int>();
    //存储群信息
    _groupModel.addGroup(userid,groupid,"normal");
}

//群聊业务
void ChatService::groupChat(const TcpConnectionPtr &conn,json &js,Timestamp){
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid,groupid);
    lock_guard<mutex> lock(_connMutex);
    for(int id : useridVec){
        auto it = _userConnMap.find(id);
        if(it != _userConnMap.end()){
            //发送消息
            it->second->send(js.dump());
        }
        else{
            //查询toid是否在线
            User User = _userModel.query(id);
            if(User.getState() == "online"){
                _redis.publish(id,js.dump());
            }
            else{
                //存储离线消息
                _offlineMessageModel.insert(id,js.dump());
            }
        }
    }
}

//注销
void ChatService::loginout(const TcpConnectionPtr &conn,json &js,Timestamp){
    int userid = js["id"].get<int>();
    //删除用户连接信息
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(userid);
        if(it != _userConnMap.end()){
            _userConnMap.erase(it);
        }
    }

    //下线，取消redis注册的通道
    _redis.unsubscribe(userid);

    //更新用户状态信息
    User user;
    user.setId(userid);
    user.setState("offline");
    _userModel.updateState(user);
}
//服务器异常，业务重置方法
void ChatService::reset(){

    //更新用户状态信息
    _userModel.resetState();

}

void ChatService::handleRedidSubScribeMessage(int id,string msg){
    lock_guard<mutex> lock(_connMutex);
    auto it = _userConnMap.find(id);
    if(it != _userConnMap.end()){
        it->second->send(msg);
        return;
    }
    //存储离线消息
    _offlineMessageModel.insert(id,msg);
}

MsgHandler ChatService::getHandler(int msgid){
    //记录错误日志
    //如果没有找到对应的处理函数，则返回一个空函数
    auto it = _HandlerMap.find(msgid);
    if(it == _HandlerMap.end()){
        
        return [=](const TcpConnectionPtr &conn,json &js,Timestamp){
            LOG_ERROR << "can't find handler for msgid:" << msgid;
        };
    }
    else{
        return _HandlerMap[msgid];
    }
}

















