#pragma once

#include "ConfigDefine.h"

class SendCache
{
  public:
    FILE* fp;                  //文件指针
    int header;                //前指针标识
    int end;                   //尾指针标识
    char cacheBuf[CACHE_WID_SEND][CACHE_LENGTH]; //缓冲区数组
    int available;             //缓冲区读写锁
    int empty;                 //缓冲区是否为空的标识
    int full;                  //缓冲区是否满的标识
    int round;                 //header和end相对位置的标识
    int lastTimeSize;          //最后一个文件块的大小
    unsigned long transTime;   //文件整块传输的次数
    int pipeLineCond;

    void zeroSpace()
    //给数组清零
    {
        for (int i = 0; i < CACHE_WID_SEND; i++)
            memset(cacheBuf[i], '/0', CACHE_LENGTH);
    }

    int judgeEmpty()
    //判断缓冲区是否为空
    //空则返回EMPTY 否则返回NOT_EMPTY
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
    //判断缓冲区是否为满
    //满则返回FULL 否则返回NOT_FULL
    {
       if((round == 1 && header == end)
        || (header == CACHE_WID_SEND  - 1 && end == 0)) 
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

class RecvCache
{
  public:
    FILE* fp;                  //文件指针
    int header;                //前指针标识
    int end;                   //尾指针标识
    char cacheBuf[CACHE_WID_RECV][CACHE_LENGTH]; //缓冲区数组
    int available;             //缓冲区读写锁
    int empty;                 //缓冲区是否为空的标识
    int full;                  //缓冲区是否满的标识
    int round;                 //header和end相对位置的标识
    int lastTimeSize;          //最后一个文件块的大小
    unsigned long transTime;   //文件整块传输的次数
    int pipeLineCond;

    void zeroSpace()
    //给数组清零
    {
        for (int i = 0; i < CACHE_WID_RECV; i++)
            memset(cacheBuf[i], '/0', CACHE_LENGTH);
    }

    int judgeEmpty()
    //判断缓冲区是否为空
    //空则返回EMPTY 否则返回NOT_EMPTY
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
    //判断缓冲区是否为满
    //满则返回FULL 否则返回NOT_FULL
    {
       if((round == 1 && header == end)
        || (header == CACHE_WID_RECV  - 1 && end == 0)) 
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
