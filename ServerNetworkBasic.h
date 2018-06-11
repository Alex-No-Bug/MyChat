#pragma once

/* 
    服务器端网络配置初始化
    服务器端基本的收发消息 */
#ifndef SERVERNETWORKBASIC_H
#define SERVERNETWORKBASIC_H
#include "ConfigDefine.h"

//--------------------------------------------------------------
//网络配置初始化函数
//完成ServerDemo_v3中while(1)循环listen之前的网络配置部分
//端口是可变的 以求实现在多个线程中的复用
int networkSetup(SOCKET* ListenSocket, SOCKET* ClientSocket, std:: string port)
{
    // using namespace std;
    WSADATA wsaData;
	int iResult;
	*ListenSocket = INVALID_SOCKET;
	*ClientSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
    
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//对Socket版本进行限制
	if (iResult != 0) {
		printf("Socket版本故障: %d\n", iResult);
		return FAIL;
	}

	ZeroMemory(&hints, sizeof(hints));
	//先清零
	hints.ai_family = AF_INET;//限定为ipv4
	hints.ai_socktype = SOCK_STREAM;//限定为stream模式传送
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;//记录进入连接的ip地址

    iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	//把服务器端口、地址和配置参数写入result hints相当于一个中介
	if (iResult != 0) {
		printf("参数初始化故障: %d\n", iResult);
		WSACleanup();
		return FAIL;
	}

    *ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//用ListenSocket来接收初始配置socket的各种信息
	if (*ListenSocket == INVALID_SOCKET) {
		printf("监听初始化故障: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return FAIL;
	}


	iResult = bind(*ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    //绑定
	if (iResult == SOCKET_ERROR) {
		printf("绑定故障: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(*ListenSocket);
		WSACleanup();
		return FAIL;
	}

    return SUCCESS;

}
//--------------------------------------------------------------


void ListenAccept(SOCKET* ListenSocket)
//监听和接收socket的部分
//对应v3：while(1)里recv之前的部分
{
    int iResult = listen(*ListenSocket, SOMAXCONN);

    //通常监听部分会写成循环 直到有连接接入为止
    if (iResult == SOCKET_ERROR)
    {
        printf("监听失败: %d\n", WSAGetLastError());
        closesocket(*ListenSocket);
        WSACleanup();
        // return 1;
    }
    //循环监听

    std:: cout << "子线程正在等待接入..." << std:: endl;
    
}
//-----------------------------------------------------------------



/* 传统意义上客户机的网络配置初始化 */
int setClinetNetwork(SOCKET* ConnectSocket)
{
    WSADATA wsaData;
    *ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    //关于初始地址模块的声明
    int iResult;

	    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup failed with error: %d\n", iResult);
        return FAIL;
    }

	ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM; //STREAM模式
    hints.ai_protocol = IPPROTO_TCP; //TCP协议
	
}

#endif
