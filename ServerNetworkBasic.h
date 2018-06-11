#pragma once

/* 
    ���������������ó�ʼ��
    �������˻������շ���Ϣ */
#ifndef SERVERNETWORKBASIC_H
#define SERVERNETWORKBASIC_H
#include "ConfigDefine.h"

//--------------------------------------------------------------
//�������ó�ʼ������
//���ServerDemo_v3��while(1)ѭ��listen֮ǰ���������ò���
//�˿��ǿɱ�� ����ʵ���ڶ���߳��еĸ���
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
	//��Socket�汾��������
	if (iResult != 0) {
		printf("Socket�汾����: %d\n", iResult);
		return FAIL;
	}

	ZeroMemory(&hints, sizeof(hints));
	//������
	hints.ai_family = AF_INET;//�޶�Ϊipv4
	hints.ai_socktype = SOCK_STREAM;//�޶�Ϊstreamģʽ����
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;//��¼�������ӵ�ip��ַ

    iResult = getaddrinfo(NULL, port.c_str(), &hints, &result);
	//�ѷ������˿ڡ���ַ�����ò���д��result hints�൱��һ���н�
	if (iResult != 0) {
		printf("������ʼ������: %d\n", iResult);
		WSACleanup();
		return FAIL;
	}

    *ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//��ListenSocket�����ճ�ʼ����socket�ĸ�����Ϣ
	if (*ListenSocket == INVALID_SOCKET) {
		printf("������ʼ������: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return FAIL;
	}


	iResult = bind(*ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    //��
	if (iResult == SOCKET_ERROR) {
		printf("�󶨹���: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(*ListenSocket);
		WSACleanup();
		return FAIL;
	}

    return SUCCESS;

}
//--------------------------------------------------------------


void ListenAccept(SOCKET* ListenSocket)
//�����ͽ���socket�Ĳ���
//��Ӧv3��while(1)��recv֮ǰ�Ĳ���
{
    int iResult = listen(*ListenSocket, SOMAXCONN);

    //ͨ���������ֻ�д��ѭ�� ֱ�������ӽ���Ϊֹ
    if (iResult == SOCKET_ERROR)
    {
        printf("����ʧ��: %d\n", WSAGetLastError());
        closesocket(*ListenSocket);
        WSACleanup();
        // return 1;
    }
    //ѭ������

    std:: cout << "���߳����ڵȴ�����..." << std:: endl;
    
}
//-----------------------------------------------------------------



/* ��ͳ�����Ͽͻ������������ó�ʼ�� */
int setClinetNetwork(SOCKET* ConnectSocket)
{
    WSADATA wsaData;
    *ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    //���ڳ�ʼ��ַģ�������
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
    hints.ai_socktype = SOCK_STREAM; //STREAMģʽ
    hints.ai_protocol = IPPROTO_TCP; //TCPЭ��
	
}

#endif
