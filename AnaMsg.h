#pragma once

/* 报文处理相关函数 */
#ifndef ANAMSG_H
#define ANAMSG_H

#include "ConfigDefine.h"
#include "User.h"
#include "Message.h"

int AnaLoginMsg(Message tempUser, UserList* userlist)
//把收到的报文先生成一个默认的用户类 可以参考报文格式表得知具体的对应关系
{
    return userlist->checkLogin(tempUser.sender, tempUser.receiver);
}
//-----------------------------------------------------------


int AnaShortMsg(Message tempMsg, MesCache* mesCache)
//如果是短消息 则直接用该报文初始化一个完全体的Message
//然后直接push进入mesCache
{
    mesCache->push_cache(tempMsg);
    return SUCCESS;
}
//------------------------------------------------------------

std:: string AnaInqShortMsg(Message msg, MesCache* mesCache)
{
    return mesCache->checkCache(msg.sender);
}

std:: string AnaInqFri(Message msg, UserList* userlist)
{
    return userlist->checkOnlineFriend(msg.sender);
}

#endif