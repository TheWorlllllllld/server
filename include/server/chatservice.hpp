#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include "../../thirdparty/json.hpp"
using json = nlohmann::json;

#include <muduo/net/TcpConnection.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <mutex>
#include <iostream>

#include "model/UserModel.hpp"
#include "model/friendmodel.hpp"
#include "model/offlinemessagemodel.hpp"
#include "model/groupmodel.hpp"
#include "redis/redis.hpp"

using namespace muduo;
using namespace muduo::net;
using namespace std;

//处理消息的回调函数的别称
using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json &js,Timestamp)>;

class ChatService{
public:
    //获取单例对象的接口函数
    static ChatService* Instance();
    //登录业务
    void login(const TcpConnectionPtr &conn,json &js,Timestamp);
    //注册业务
    void registerUser(const TcpConnectionPtr &conn,json &js,Timestamp);
    //单聊业务
    void oneChat(const TcpConnectionPtr &conn,json &js,Timestamp);
    //添加好友
    void addFriend(const TcpConnectionPtr &conn,json &js,Timestamp);  
    //创建群
    void createGroup(const TcpConnectionPtr &conn,json &js,Timestamp);
    //加入群
    void addGroup(const TcpConnectionPtr &conn,json &js,Timestamp);
    //群聊业务
    void groupChat(const TcpConnectionPtr &conn,json &js,Timestamp);
    //注销
    void loginout(const TcpConnectionPtr &conn,json &js,Timestamp);
    //获取消息对应处理函数
    MsgHandler getHandler(int msgType);
    //服务器异常，业务重置方法
    void reset();
    //处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    //redis上报新消息
    void handleRedidSubScribeMessage(int,string);
private:
    ChatService();
    
    //存储消息id和对应的业务处理方法(回调函数)
    unordered_map<int,MsgHandler> _HandlerMap;

    //数据操作类对象
    UserModel _userModel;
    FreindModel _friendModel;
    OfflineMessageModel _offlineMessageModel;
    GroupModel _groupModel;
    //定义互斥锁
    std::mutex _connMutex;

    //存储在线用户的通信连接
    unordered_map<int,TcpConnectionPtr> _userConnMap;
    //redis对象
    Redis _redis;
};


#endif