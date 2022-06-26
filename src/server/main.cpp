#include "../../include/server/chatserver.hpp"
#include "../../include/server/chatservice.hpp"
#include <iostream>
#include <signal.h>
using namespace std;

//处理服务器ctrl+c信号
void resetHandler(int sig){
    ChatService::Instance()->reset();
    exit(0);
}

int main(int argc, char **argv){
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT,resetHandler);
    EventLoop loop;
    InetAddress addr(ip,port);

    Chatserver server(&loop,addr,"ChatServer");
    server.start();
    loop.loop();
    
    return 0;
}