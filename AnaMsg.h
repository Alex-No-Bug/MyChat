#pragma once

/* ���Ĵ�����غ��� */
#ifndef ANAMSG_H
#define ANAMSG_H

#include "ConfigDefine.h"
#include "User.h"
#include "Message.h"

int AnaLoginMsg(Message tempUser, UserList* userlist)
//���յ��ı���������һ��Ĭ�ϵ��û��� ���Բο����ĸ�ʽ���֪����Ķ�Ӧ��ϵ
{
    return userlist->checkLogin(tempUser.sender, tempUser.receiver);
}
//-----------------------------------------------------------


int AnaShortMsg(Message tempMsg, MesCache* mesCache)
//����Ƕ���Ϣ ��ֱ���øñ��ĳ�ʼ��һ����ȫ���Message
//Ȼ��ֱ��push����mesCache
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