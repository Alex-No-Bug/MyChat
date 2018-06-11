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
    //�û����ݿ⡢��Ϣ���ݿ⡢�ļ���Ϣ���ݿ��ʼ��
    //----------------------------------------------------------------

    SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
    WSADATA wsaData;
	int iResult;
	struct addrinfo *result = NULL;
	struct addrinfo hints;
    
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//��Socket�汾��������
	if (iResult != 0) {
		printf("Socket�汾����: %d\n", iResult);
		// return FAIL;
	}

	ZeroMemory(&hints, sizeof(hints));
	//������
	hints.ai_family = AF_INET;//�޶�Ϊipv4
	hints.ai_socktype = SOCK_STREAM;//�޶�Ϊstreamģʽ����
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;//��¼�������ӵ�ip��ַ

    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	//�ѷ������˿ڡ���ַ�����ò���д��result hints�൱��һ���н�
	if (iResult != 0) {
		printf("������ʼ������: %d\n", iResult);
		WSACleanup();
		// return FAIL;
	}

    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//��ListenSocket�����ճ�ʼ����socket�ĸ�����Ϣ
	if (ListenSocket == INVALID_SOCKET) {
		printf("������ʼ������: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		// return FAIL;
	}


	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    //��
	if (iResult == SOCKET_ERROR) {
		printf("�󶨹���: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		// return FAIL;
	}
    std:: cout << "�����ʼ�����" << std:: endl;
    //-----�������ó�ʼ��---------------------------------------------
    //--------------------------------------------------------------

    char recvBuf[8800] = {0};

    InqThreadClass inquiry(&userlist, &mesCache, &filelist);
    HANDLE inqHandle = CreateThread(NULL, 0, InqThread,
                                   (void *)&inquiry, 0, NULL);
    CloseHandle(inqHandle);
    //-----------�����߳�����----------------------------------------
    //-------------------------------------------------------------

    while(1)
    {
        int iResult = listen(ListenSocket, SOMAXCONN);

        //ͨ���������ֻ�д��ѭ�� ֱ�������ӽ���Ϊֹ
        if (iResult == SOCKET_ERROR)
        {
            printf("����ʧ��: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            // return 1;
        }
        //ѭ������

        std::cout << "���ڵȴ�����..." << std::endl;
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("����ʧ��: %d\n", WSAGetLastError());
            continue;
            return 1;
        }
        std::cout << "�յ�һ��SOCKET: " << std::endl;
        //---------------�����ͶԽ�socket---------------------------
        //-----------------------------------------------------------

        std::memset(recvBuf, '\0', DEFAULT_BUFLEN);
        iResult = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        std:: string rawMsg = recvBuf;
        //------------------����ͽ����ļ�-----------------------------

        if(iResult > 0)
        /* ������
            main����Ҫ������ǵ�¼���� ����Ϣ����
            �����ɹ�����һ��string"OK"
            ���򷵻�һ��string"FAIL"
            Ҫ���������ݿ⣺
            �û���Ϣ���ݿ⣨���ҡ�ƥ���д�룩 ��Ϣ�������ݿ⣨д�룩 */
        {
            std:: cout << "�յ�һ������" << std:: endl;
            std:: cout << recvBuf << std::endl;
            std:: string retVal = "X";
            Message curMsg (rawMsg);//decode
            //----------------��ر����ĳ�ʼ����ǿ������-------------
            //-----------------------------------------------------

            if(curMsg.msgFlag == LOGIN_INFO)
            {
                if (AnaLoginMsg(curMsg, &userlist) == SUCCESS)
                {
                    retVal = "OK";
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //���͵�¼������Ϣ
                    retVal = userlist.getFri(curMsg.sender);
                    //�õ����к�������
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //���ͺ�������
                }    
                else
                {
                    retVal = "FAIL";
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                }
            }
            //-------------�����¼����-----------------------
            //------------------------------------------------

            else if(curMsg.msgFlag == SHORT_MSG)
            {
                if(AnaShortMsg(curMsg, &mesCache) == SUCCESS)
                    retVal = "OK";
                else 
                    retVal = "FAIL";
                send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                //���ͷ�����Ϣ
            }
            //-------------�������Ϣ����--------------------------
            //----------------------------------------------------
            
            else if(curMsg.msgFlag == FILE_MSG)
            {
                FileServer curFile(curMsg);
                curFile.path = "E:\\" + curFile.fileName;
                send(ClientSocket, "OK", 3, 0);
                /* ��ʼ��FileServer
                ����Ĭ��·��
                ���ͻ��˷���ACK */
                //-----------------------------------------------

                std:: FILE* fp = fopen((curFile.path).c_str(), "wb");
                //���ļ�
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
                //����������������ʼ��
                //---------------------------------------------------------
                
                HANDLE pipeLine = CreateThread(NULL, 0, RecvPipeline,
                                               (void *)&cache, 0, NULL);
                CloseHandle(pipeLine);
                //���߳�����
                //-------------------------------------------------------

                unsigned long curRecvCount = 0;
                while (curRecvCount < curFile.transCount)
                {
                    while (cache.judgeFull() == NOT_FULL && curRecvCount < curFile.transCount)
                    //ֻҪ����λ�ͷ������
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
                        //�������
                        //�ļ�����
                        //�Ķ�д���λ
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
                //����������
                //----------------------------------------------------------------

                memset(cache.cacheBuf[cache.header], '\0', CACHE_LENGTH);
                while (cache.judgeFull() == FULL)
                    ;
                //������վ�һֱ������
                int iResult = recv(ClientSocket, cache.cacheBuf[cache.header],
                               curFile.lastTimeSize, 0);
                if (cache.header == CACHE_WID_RECV - 1)
                {
                    cache.round = 1;
                    cache.header = 0;
                }
                else
                    cache.header++;
                //���һ�ν���
                //�ļ�����
                //�Ķ�д���λ
                //socket��ʱ�������� ���Ķ�д���λ�ǲ����ܵ�
                //--------------------------------------------------------
            }
            //�����ļ�����
            //�����ļ��߳�
            //---------------------------------------------------------------------
            else;
        }
        else if (iResult == 0)
            printf("���Ӽ����ر�");
        else
        {
            printf("���ӷ�������%d\n", WSAGetLastError());
            Sleep(10000);
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    }
    system("pause");
}