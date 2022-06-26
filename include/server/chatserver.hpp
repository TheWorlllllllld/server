#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>

#include <string>
#include <iostream>

using namespace muduo;
using namespace muduo::net;
// using namespace placeholders;
//server主类
class Chatserver
{
public:
    //初始化
    Chatserver(EventLoop *loop,
            const InetAddress& listenAddr,
            const string nameArg);
    //启动服务
    void start();
private:
    //上报连接相关信息的回调函数
    void onConnection(const TcpConnectionPtr& conn);
    //上报读写事件相关信息的回调函数
    void onMessage(const TcpConnectionPtr& conn,
            Buffer* buf,
            Timestamp receiveTime);
            

    TcpServer _server;//组合的muduo库，实现server功能的类对象
    EventLoop *_loop;//指向一个EventLoop对象的指针 
};

#endif