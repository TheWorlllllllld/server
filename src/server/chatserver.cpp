#include "../../include/server/chatserver.hpp"
#include "../../thirdparty/json.hpp"
#include "../../include/server/chatservice.hpp"

using namespace std;
using json = nlohmann::json;

Chatserver::Chatserver(EventLoop *loop,
                       const InetAddress &listenAddr,
                       const string nameArg) : _server(loop, listenAddr, nameArg)
{
    //注册连接回调
    _server.setConnectionCallback(bind(&Chatserver::onConnection, this, _1));
    //注册消息回调
    _server.setMessageCallback(bind(&Chatserver::onMessage, this, _1, _2, _3));
    _server.setThreadNum(4);
}

void Chatserver::start()
{
    _server.start();
}

//上报连接相关信息的回调函数
void Chatserver::onConnection(const TcpConnectionPtr &conn)
{
    //客户端断开连接
    if (conn->connected())
    {
        printf("onConnection(): new connection [%s] from %s \n",
               conn->name().c_str(),
               conn->peerAddress().toIpPort().c_str());
    }
    else
    {
        ChatService::Instance()->clientCloseException(conn);
        printf("onConnection() : connection [%s] is down\n",
               conn->name().c_str());
        conn->shutdown();
    }
}
//上报读写事件相关信息的回调函数
void Chatserver::onMessage(const TcpConnectionPtr &conn,
                           Buffer *buf,
                           Timestamp receiveTime)
{
    string _buf = buf->retrieveAllAsString();
    //反序列化
    json js = json::parse(_buf);
    //获取消息类型  
    auto msgHandler = ChatService::Instance()->getHandler(js["msgid"].get<int>());
    msgHandler(conn, js, receiveTime);
}