#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;


#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")
#pragma comment(lib, "AdvApi32.lib")

#include "User.h"
#include "ConfigDefine.h"
#include "Message.h"
#include "AnaMsg.h"
#include "FileServer.h"
#include "SubThread.h"


int main()
{
    UserList userlist;
    userlist.init();
    FileList filelist;
    MesCache mesCache;
    //用户数据库、消息数据库、文件消息数据库初始化
    //----------------------------------------------------------------

    SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
    WSADATA wsaData;
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
    
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//对Socket版本进行限制
	if (iResult != 0) {
		printf("Socket版本故障: %d\n", iResult);
		// return FAIL;
	}

	ZeroMemory(&hints, sizeof(hints));
	//先清零
	hints.ai_family = AF_INET;//限定为ipv4
	hints.ai_socktype = SOCK_STREAM;//限定为stream模式传送
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;//记录进入连接的ip地址

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	//把服务器端口、地址和配置参数写入result hints相当于一个中介
	if (iResult != 0) {
		printf("参数初始化故障: %d\n", iResult);
		WSACleanup();
		// return FAIL;
	}

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//用ListenSocket来接收初始配置socket的各种信息
	if (ListenSocket == INVALID_SOCKET) {
		printf("监听初始化故障: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		// return FAIL;
	}


	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    //绑定
	if (iResult == SOCKET_ERROR) {
		printf("绑定故障: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		// return FAIL;
	}
    std:: cout << "网络初始化完成" << std:: endl;
    //-----网络设置初始化---------------------------------------------
    //--------------------------------------------------------------

    char recvBuf[8800] = {0};

    InqThreadClass inquiry(&userlist, &mesCache, &filelist);
    HANDLE inqHandle = CreateThread(NULL, 0, InqThread,
                                   (void *)&inquiry, 0, NULL);
    CloseHandle(inqHandle);
    //-----------监听线程启动----------------------------------------
    //-------------------------------------------------------------

    while(1)
    {
        int iResult = listen(ListenSocket, SOMAXCONN);

        //通常监听部分会写成循环 直到有连接接入为止
        if (iResult == SOCKET_ERROR)
        {
            printf("监听失败: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            // return 1;
        }
        //循环监听

        std::cout << "正在等待接入..." << std::endl;
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("接收失败: %d\n", WSAGetLastError());
            continue;
            return 1;
        }
        std::cout << "收到一个SOCKET: " << std::endl;
        //---------------监听和对接socket---------------------------
        //-----------------------------------------------------------

        std::memset(recvBuf, '\0', DEFAULT_BUFLEN);
        iResult = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        std:: string rawMsg = recvBuf;
        //------------------清零和接收文件-----------------------------

        if(iResult > 0)
        /* 处理报文
            main函数要处理的是登录报文 短消息报文
            操作成功返回一个string"OK"
            否则返回一个string"FAIL"
            要操作的数据库：
            用户信息数据库（查找、匹配和写入） 消息缓冲数据库（写入） */
        {
            std:: cout << "收到一个报文" << std:: endl;
            std:: cout << recvBuf << std::endl;
            std:: string retVal = "X";
            Message curMsg (rawMsg);//decode
            //----------------相关变量的初始化和强制译码-------------
            //-----------------------------------------------------

            if(curMsg.msgFlag == LOGIN_INFO)
            {
                if (AnaLoginMsg(curMsg, &userlist) == SUCCESS)
                {
                    retVal = "OK";
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //发送登录反馈消息
                    retVal = userlist.getFri(curMsg.sender);
                    //得到所有好友名单
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //发送好友名单
                }    
                else
                {
                    retVal = "FAIL";
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                }
            }
            //-------------处理登录报文-----------------------
            //------------------------------------------------

            else if(curMsg.msgFlag == SHORT_MSG)
            {
                if(AnaShortMsg(curMsg, &mesCache) == SUCCESS)
                    retVal = "OK";
                else 
                    retVal = "FAIL";
                send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                //发送反馈消息
            }
            //-------------处理短消息报文--------------------------
            //----------------------------------------------------
            
            else if(curMsg.msgFlag == FILE_MSG)
            {
                FileServer curFile(curMsg);
                curFile.path = "E:\\" + curFile.fileName;
                send(ClientSocket, "OK", 3, 0);
                /* 初始化FileServer
                设置默认路径
                给客户端发送ACK */
                //-----------------------------------------------

                std:: FILE* fp = fopen((curFile.path).c_str(), "wb");
                //打开文件
                CacheRecv cache;
                cache.fp = fp;
                cache.header = cache.end = cache.round = 0;
                cache.empty = EMPTY;
                cache.full = NOT_FULL;
                cache.available = UNLOCK;
                cache.zeroSpace();
                cache.pipeLineCond = NOT_DONE;
                cache.lastTimeSize = curFile.lastTimeSize;
                cache.transTime = curFile.transCount;
                //缓冲区类对象参数初始化
                //---------------------------------------------------------
                
                HANDLE pipeLine = CreateThread(NULL, 0, RecvPipeline,
                                               (void *)&cache, 0, NULL);
                CloseHandle(pipeLine);
                //多线程启动
                //-------------------------------------------------------

                unsigned long curRecvCount = 0;
                while (curRecvCount < curFile.transCount)
                {
                    while (cache.judgeFull() == NOT_FULL && curRecvCount < curFile.transCount)
                    //只要给空位就疯狂的输出
                    {
                        // cout << curRecvCount << " " << cache.header << " " << cache.end << " " << cache.round;
                        int iResult = recv(ClientSocket, cache.cacheBuf[cache.header], CACHE_LENGTH, MSG_WAITALL);
                        curRecvCount++;
                        if (cache.header == CACHE_WID_RECV - 1)
                        {
                            cache.round = 1;
                            cache.header = 0;
                        }
                        else
                            cache.header++;
                        //常规接收
                        //改计数量
                        //改读写标记位
                        //--------------------------------------------------------

                        if (iResult < 0)
                        {
                            printf("recv failed with error: %d\n", WSAGetLastError());
                            closesocket(ClientSocket);
                            //Sleep(5000);
                            WSACleanup();
                            return 1;
                        }
                    }
                }
                //常规接收完毕
                //----------------------------------------------------------------

                memset(cache.cacheBuf[cache.header], '\0', CACHE_LENGTH);
                while (cache.judgeFull() == FULL)
                    ;
                //如果不空就一直等着呗
                int iResult = recv(ClientSocket, cache.cacheBuf[cache.header],
                               curFile.lastTimeSize, 0);
                if (cache.header == CACHE_WID_RECV - 1)
                {
                    cache.round = 1;
                    cache.header = 0;
                }
                else
                    cache.header++;
                //最后一次接收
                //改计数量
                //改读写标记位
                //socket此时是生产者 不改读写标记位是不可能的
                //--------------------------------------------------------
            }
            //处理文件报文
            //调用文件线程
            //---------------------------------------------------------------------
            else;
        }
        else if (iResult == 0)
            printf("连接即将关闭");
        else
        {
            printf("连接发生错误%d\n", WSAGetLastError());
            Sleep(10000);
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }
    system("pause");
}