#ifndef PUBLIC_H
#define PUBLIC_H

//server和client公共文件

enum EnMsgType
{
    LOGIN_MSG = 1,//登录
    LOGIN_MSG_ACK,//登录回复
    REG_MSG,    //注册
    REG_MSG_ACK,//注册确认
    ONE_CHAT_MSG, //单聊
    ADD_FRIEND_MSG, //添加好友

    CREATE_GROUP_MSG,//创建群
    ADD_GROUP_MSG,//添加群
    GROUP_CHAT_MSG,//群聊
    LOGINOUT_MSG,//登出
};



#endif