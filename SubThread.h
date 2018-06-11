#pragma once

/* 所有子线程的函数都在该文件中 */
#ifndef SUBTHREAD_H
#define SUBTHREAD_H

#include "ServerNetworkBasic.h"
#include "SubThreadClass.h"
#include "AnaMsg.h"

DWORD WINAPI SendPipeline(LPVOID para);

/* 监听子线程
    初始化网络
    得到一个报文 首先强制译码
    查询短消息缓冲数据库 生成一个string报文并发送
    查询用户信息数据库 查询当前在线好友 生成一个string报文发送
    查询文件缓冲数据库 发送文件接收确认 然后调用文件发送进程 直到所有文件都被发送 */

DWORD WINAPI InqThread(LPVOID para)
{
    InqThreadClass* inquiry = (InqThreadClass*) para;
    char recvBuf[DEFAULT_BUFLEN] = {0};

    SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
    int res = networkSetup(&ListenSocket, &ClientSocket, INQUIRY_PORT);
    if(res == FAIL) 
    {
        std:: cout << "初始化异常" << std::endl;
        Sleep(10000);
        return 1;
    }
    std:: cout << "询问网络初始化完成" << std:: endl;
    //-----网络设置初始化---------------------------------------------
    //--------------------------------------------------------------
    
    while(1)
    {
        ListenAccept(&ListenSocket);

        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("子线程接收失败: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            // WSACleanup();
            return 1;
        }
        std::cout << "子线程收到一个SOCKET: " << std::endl;
        //---------------监听和对接socket---------------------------
        //-----------------------------------------------------------

        std::memset(recvBuf, '\0', DEFAULT_BUFLEN);
        res = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        std:: string rawMsg = recvBuf;
        //------------------清零和接收文件-----------------------------

        if(res > 0)
        /* 处理报文
            强制译码
            查询短消息数据库并发送消息
            查询用户数据库并发送消息
            查询用户文件数据库并发送消息 */
        {
            std:: cout << "子线程收到一个报文" << std:: endl;
            std:: cout << recvBuf << std::endl;
            std:: string retVal = "";
            Message curMsg (rawMsg);//decode
            //----------------相关变量的初始化和强制译码-------------
            //-----------------------------------------------------

            retVal = AnaInqShortMsg(rawMsg, inquiry->mesCache);
            send(ClientSocket, retVal.c_str(), retVal.length(), 0);
            cout << retVal << endl;
            //发送当前待收短消息
            //短消息报文都是以*为开头的
            //---------------------------------------------------------

            retVal = AnaInqFri(rawMsg, inquiry->userlist);
            send(ClientSocket, retVal.c_str(), retVal.length(), 0);
            cout << "在线好友 " << retVal << endl;
            //发送当前在线好友
            //好友名单都是以|为开头的
            //-------------------------------------------------------------   

            FileServer checkRes = inquiry->filelist->check(curMsg.sender);
            //查询文件
            //发送文件还是用的INQUIRY_PORT
            if(checkRes.path == "EMPTY") send(ClientSocket, "EMPTY", 6, 0);
            //如果是空 就直接告诉客户端 以免客户端阻塞

            cout << "检查文件了" << endl;
            while(checkRes.path != "EMPTY")
            {
                std:: string retVal = checkRes.toString();

                if(checkRes.fileOrGra == GRAPH_MSG)
                //区分是表情包还是普通文件
                {
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //给用户发送确认信息
                    cout << retVal << endl;
                    std:: memset(recvBuf, '\0', DEFAULT_BUFLEN);
                    recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
                    //收到用户回执
                    //-------------------------------------------------------

                    CacheSend cache;
                    std::FILE* fp = fopen((checkRes.path + checkRes.fileName).c_str(), "rb");
                    cache.fp = fp;
                    cache.header = cache.end = cache.round = 0;
                    cache.empty = EMPTY;
                    cache.full = NOT_FULL;
                    cache.available = UNLOCK;
                    cache.zeroSpace();
                    cache.pipeLineCond = NOT_DONE;
                    cache.lastTimeSize = checkRes.lastTimeSize;
                    cache.transTime = checkRes.transCount;
                    //类对象参数初始化
                    //-----------------------------------------------------

                    //----------------------------------------------------
                    //---------------发送核心模块----------------------------
                    //------------------------------------------------------
                    HANDLE pipeline = CreateThread(NULL, 0, SendPipeline,
                                                   (void *)&cache, 0, NULL);
                    CloseHandle(pipeline);
                    //线程启动
                    //-------------------------------------------

                    unsigned long curTransCount = 0;
                    while (curTransCount < checkRes.transCount)
                    {
                        while (cache.judgeEmpty() == NOT_EMPTY && curTransCount < checkRes.transCount)
                        //只要有东西就疯狂的输出
                        {
                            // cout << curTransCount << endl;
                            // cout << cache.header << " " << cache.end << " " << cache.round;
                            int iResult = send(ClientSocket, cache.cacheBuf[cache.end], CACHE_LENGTH, 0);
                            //在发送端socket相当于缓冲区的消费者
                            if (cache.end == CACHE_WID_SEND - 1)
                            {
                                cache.round = 0;
                            }
                            cache.end = (cache.end + 1) % CACHE_WID_SEND;
                            //修改读写标志位
                            //------------------------------------------------------------------------

                            curTransCount++;
                            if (iResult == SOCKET_ERROR)
                            {
                                printf("内部循环: %d ", WSAGetLastError());
                                std::cout << curTransCount << std::endl;
                                Sleep(5000);
                                // closesocket(ConnectSocket);
                                WSACleanup();
                                return 1;
                            }
                        }
                    }
                    //--------------------------------------------------------------------------
                    //-------------------------规整的整块传送------------------------------------
                    //---------------------------------------------------------------------------
                    while (cache.judgeEmpty() == EMPTY)
                        ;
                    std::cout << "开启最后的传送" <<std:: endl;
                    int iResult = send(ClientSocket, cache.cacheBuf[cache.end], cache.lastTimeSize, 0);
                    if (iResult == SOCKET_ERROR)
                    {
                        printf("最后一次失败: %d\n", WSAGetLastError());
                        WSACleanup();
                        return 1;
                    }
                    std::cout << "发送结束" << std::endl;
                    //发送最后一片
                    //------------------------------------------------------------------
                }

                
                else
                //如果是需要得到确认回执的文件
                {
                    cout << retVal << endl;
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //给用户发送确认信息
                    std::memset(recvBuf, '\0', DEFAULT_BUFLEN);
                    recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
                    //收到用户回执
                    if (recvBuf[0] == YES)
                    {
                        CacheSend cache;
                        std::FILE* fp = fopen((checkRes.path + checkRes.fileName).c_str(), "rb");
                        cache.fp = fp;
                        cache.header = cache.end = cache.round = 0;
                        cache.empty = EMPTY;
                        cache.full = NOT_FULL;
                        cache.available = UNLOCK;
                        cache.zeroSpace();
                        cache.pipeLineCond = NOT_DONE;
                        cache.lastTimeSize = checkRes.lastTimeSize;
                        cache.transTime = checkRes.transCount;
                        //类对象参数初始化
                        //-----------------------------------------------------

                        //----------------------------------------------------
                        //---------------发送核心模块----------------------------
                        //------------------------------------------------------
                        HANDLE pipeline = CreateThread(NULL, 0, SendPipeline,
                                                       (void *)&cache, 0, NULL);
                        CloseHandle(pipeline);
                        //线程启动
                        //-------------------------------------------

                        unsigned long curTransCount = 0;
                        while (curTransCount < checkRes.transCount)
                        {
                            while (cache.judgeEmpty() == NOT_EMPTY && curTransCount < checkRes.transCount)
                            //只要有东西就疯狂的输出
                            {
                                // cout << curTransCount << endl;
                                // cout << cache.header << " " << cache.end << " " << cache.round;
                                int iResult = send(ClientSocket, cache.cacheBuf[cache.end], CACHE_LENGTH, 0);
                                //在发送端socket相当于缓冲区的消费者
                                if (cache.end == CACHE_WID_SEND - 1)
                                {
                                    cache.round = 0;
                                }
                                cache.end = (cache.end + 1) % CACHE_WID_SEND;
                                //修改读写标志位
                                //------------------------------------------------------------------------

                                curTransCount++;
                                if (iResult == SOCKET_ERROR)
                                {
                                    printf("内部循环: %d ", WSAGetLastError());
                                    std::cout << curTransCount << std::endl;
                                    Sleep(5000);
                                    // closesocket(ConnectSocket);
                                    WSACleanup();
                                    return 1;
                                }
                            }
                        }
                        //--------------------------------------------------------------------------
                        //-------------------------规整的整块传送------------------------------------
                        //---------------------------------------------------------------------------
                        while (cache.judgeEmpty() == EMPTY)
                            ;
                        std::cout << "开启最后的传送" << std::endl;
                        int iResult = send(ClientSocket, cache.cacheBuf[cache.end], cache.lastTimeSize, 0);
                        if (iResult == SOCKET_ERROR)
                        {
                            printf("最后一次失败: %d\n", WSAGetLastError());
                            WSACleanup();
                            return 1;
                        }
                        std::cout << "发送结束" << std::endl;
                        //发送最后一片
                        //------------------------------------------------------------------
                        //如果有且得到用户确认则调用文件传输多线程
                    }
                }

                FileServer checkRes = inquiry->filelist->check(curMsg.sender);
            }
        }
        else if (res == 0)
            printf("子线程连接即将关闭");
        else
        {
            printf("子线程连接发生错误", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }


}
//---------------------------------------------------------


DWORD WINAPI SendPipeline(LPVOID para)
{
    CacheSend* cachePtr = (CacheSend*) para;
    unsigned long count = 0;

    while(count < cachePtr -> transTime)
    {
       while(cachePtr->judgeFull() == NOT_FULL && count < cachePtr -> transTime)
       //只要有空就疯狂的输出
       {
           fread(cachePtr->cacheBuf[cachePtr->header],
                 CACHE_LENGTH, 1, cachePtr->fp);
           if (cachePtr->header == CACHE_WID_SEND - 1)
           {
               cachePtr->round = 1;
               cachePtr->header = 0;
           }
           else
               cachePtr->header++;
           count++;
       }

    }   
    //----------------------------------------------------------------
    //------------------------规整的整块写入-----------------------------
    //-----------------------------------------------------------------
    
    memset(cachePtr->cacheBuf[cachePtr->header], '\0', CACHE_LENGTH);
    //清零当前块
    fread(cachePtr->cacheBuf[cachePtr->header], cachePtr->lastTimeSize,
           1, cachePtr->fp);
    fclose(cachePtr->fp);
    if (cachePtr->header == CACHE_WID_SEND - 1)
    {
        cachePtr->round = 1;
        cachePtr->header = 0;
    }
    else
        cachePtr->header++;
    std:: cout << "done write" << std::endl;
    //---------------------------------------------------------------
    //最后一片的写入
    //--------------------------------------------------------------
	return 0;
}
//----------------------------------------------------------------



DWORD WINAPI RecvPipeline(LPVOID para)
{
    CacheRecv* cachePtr = (CacheRecv*) para;
    
    unsigned long count = 0;

    while(count < cachePtr -> transTime )
    {
        if (cachePtr -> judgeEmpty() == NOT_EMPTY && count < cachePtr -> transTime)
        {
            cout << "count:  " << count << endl;
            fwrite(cachePtr->cacheBuf[cachePtr->end], CACHE_LENGTH, 1, cachePtr->fp);
            if(cachePtr -> end == CACHE_WID_RECV - 1) cachePtr -> round = 0;
            cachePtr -> end = (cachePtr -> end + 1) % CACHE_WID_RECV;
            count ++;
        }
        //-----------------------------------------------------
        //适当控制写入文件的速度 为接收端提供更多时间 每次读完都要修改end
        //循环结束调用两个函数 修改空满标志量
        //----------------------------------------------------
    }
    //-------------------整块传送完了------------------------------
    
    while(cachePtr -> judgeEmpty() == EMPTY) ;//等待最后一片写进来
    fwrite(cachePtr->cacheBuf[cachePtr->end], cachePtr->lastTimeSize,
           1, cachePtr->fp);
    fclose(cachePtr->fp);
    cachePtr->pipeLineCond = DONE;
    std:: cout << "done save" << std:: endl;
    return 0;
}



#endif