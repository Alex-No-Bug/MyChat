#pragma once

#ifndef SUBTHREADCLASS_H
#define SUBTHREADCLASS_H

#include "ConfigDefine.h"
#include "User.h"
#include "Message.h"
#include "FileServer.h"

class InqThreadClass
{
    public:
        UserList* userlist;
        MesCache* mesCache;
        FileList* filelist;

    InqThreadClass (UserList* u, MesCache* m, FileList* f)
    {
        userlist = u;
        mesCache = m;
        filelist = f;
    } 

};


class CacheSend
{
  public:
    FILE* fp;                  //�ļ�ָ��
    int header;                //ǰָ���ʶ
    int end;                   //βָ���ʶ
    char cacheBuf[CACHE_WID_SEND][CACHE_LENGTH]; //����������
    int available;             //��������д��
    int empty;                 //�������Ƿ�Ϊ�յı�ʶ
    int full;                  //�������Ƿ����ı�ʶ
    int round;                 //header��end���λ�õı�ʶ
    int lastTimeSize;          //���һ���ļ���Ĵ�С
    unsigned long transTime;   //�ļ����鴫��Ĵ���
    int pipeLineCond;

    void zeroSpace()
    //����������
    {
        for (int i = 0; i < CACHE_WID_SEND; i++)
            memset(cacheBuf[i], '/0', CACHE_LENGTH);
    }

    int judgeEmpty()
    //�жϻ������Ƿ�Ϊ��
    //���򷵻�EMPTY ���򷵻�NOT_EMPTY
    {
        if(round == 0 && header == end) 
        {
            empty = EMPTY;
            return EMPTY;
        }
        else 
        {
            empty = NOT_EMPTY;
            return NOT_EMPTY;
        }
    }

    int judgeFull()
    //�жϻ������Ƿ�Ϊ��
    //���򷵻�FULL ���򷵻�NOT_FULL
    {
       if(round == 1 && header == end) 
       {
           full = FULL;
           return FULL;
       }
       else 
       {
           full = NOT_FULL;
           return NOT_FULL;
       }
    }


};

class CacheRecv
{
  public:
    FILE* fp;                  //�ļ�ָ��
    int header;                //ǰָ���ʶ
    int end;                   //βָ���ʶ
    char cacheBuf[CACHE_WID_RECV][CACHE_LENGTH]; //����������
    int available;             //��������д��
    int empty;                 //�������Ƿ�Ϊ�յı�ʶ
    int full;                  //�������Ƿ����ı�ʶ
    int round;                 //header��end���λ�õı�ʶ
    int lastTimeSize;          //���һ���ļ���Ĵ�С
    unsigned long transTime;   //�ļ����鴫��Ĵ���
    int pipeLineCond;

    void zeroSpace()
    //����������
    {
        for (int i = 0; i < CACHE_WID_RECV; i++)
            memset(cacheBuf[i], '/0', CACHE_LENGTH);
    }

    int judgeEmpty()
    //�жϻ������Ƿ�Ϊ��
    //���򷵻�EMPTY ���򷵻�NOT_EMPTY
    {
        if(round == 0 && header == end) 
        {
            empty = EMPTY;
            return EMPTY;
        }
        else 
        {
            empty = NOT_EMPTY;
            return NOT_EMPTY;
        }
    }

    int judgeFull()
    //�жϻ������Ƿ�Ϊ��
    //���򷵻�FULL ���򷵻�NOT_FULL
    {
       if(round == 1 && header == end) 
       {
           full = FULL;
           return FULL;
       }
       else 
       {
           full = NOT_FULL;
           return NOT_FULL;
       }
    }


};


#endif