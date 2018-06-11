#pragma once

/* �������̵߳ĺ������ڸ��ļ��� */
#ifndef SUBTHREAD_H
#define SUBTHREAD_H

#include "ServerNetworkBasic.h"
#include "SubThreadClass.h"
#include "AnaMsg.h"

DWORD WINAPI SendPipeline(LPVOID para);

/* �������߳�
    ��ʼ������
    �õ�һ������ ����ǿ������
    ��ѯ����Ϣ�������ݿ� ����һ��string���Ĳ�����
    ��ѯ�û���Ϣ���ݿ� ��ѯ��ǰ���ߺ��� ����һ��string���ķ���
    ��ѯ�ļ��������ݿ� �����ļ�����ȷ�� Ȼ������ļ����ͽ��� ֱ�������ļ��������� */

DWORD WINAPI InqThread(LPVOID para)
{
    InqThreadClass* inquiry = (InqThreadClass*) para;
    char recvBuf[DEFAULT_BUFLEN] = {0};

    SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
    int res = networkSetup(&ListenSocket, &ClientSocket, INQUIRY_PORT);
    if(res == FAIL) 
    {
        std:: cout << "��ʼ���쳣" << std::endl;
        Sleep(10000);
        return 1;
    }
    std:: cout << "ѯ�������ʼ�����" << std:: endl;
    //-----�������ó�ʼ��---------------------------------------------
    //--------------------------------------------------------------
    
    while(1)
    {
        ListenAccept(&ListenSocket);

        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET)
        {
            printf("���߳̽���ʧ��: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            // WSACleanup();
            return 1;
        }
        std::cout << "���߳��յ�һ��SOCKET: " << std::endl;
        //---------------�����ͶԽ�socket---------------------------
        //-----------------------------------------------------------

        std::memset(recvBuf, '\0', DEFAULT_BUFLEN);
        res = recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
        std:: string rawMsg = recvBuf;
        //------------------����ͽ����ļ�-----------------------------

        if(res > 0)
        /* ������
            ǿ������
            ��ѯ����Ϣ���ݿⲢ������Ϣ
            ��ѯ�û����ݿⲢ������Ϣ
            ��ѯ�û��ļ����ݿⲢ������Ϣ */
        {
            std:: cout << "���߳��յ�һ������" << std:: endl;
            std:: cout << recvBuf << std::endl;
            std:: string retVal = "";
            Message curMsg (rawMsg);//decode
            //----------------��ر����ĳ�ʼ����ǿ������-------------
            //-----------------------------------------------------

            retVal = AnaInqShortMsg(rawMsg, inquiry->mesCache);
            send(ClientSocket, retVal.c_str(), retVal.length(), 0);
            cout << retVal << endl;
            //���͵�ǰ���ն���Ϣ
            //����Ϣ���Ķ�����*Ϊ��ͷ��
            //---------------------------------------------------------

            retVal = AnaInqFri(rawMsg, inquiry->userlist);
            send(ClientSocket, retVal.c_str(), retVal.length(), 0);
            cout << "���ߺ��� " << retVal << endl;
            //���͵�ǰ���ߺ���
            //��������������|Ϊ��ͷ��
            //-------------------------------------------------------------   

            FileServer checkRes = inquiry->filelist->check(curMsg.sender);
            //��ѯ�ļ�
            //�����ļ������õ�INQUIRY_PORT
            if(checkRes.path == "EMPTY") send(ClientSocket, "EMPTY", 6, 0);
            //����ǿ� ��ֱ�Ӹ��߿ͻ��� ����ͻ�������

            cout << "����ļ���" << endl;
            while(checkRes.path != "EMPTY")
            {
                std:: string retVal = checkRes.toString();

                if(checkRes.fileOrGra == GRAPH_MSG)
                //�����Ǳ����������ͨ�ļ�
                {
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //���û�����ȷ����Ϣ
                    cout << retVal << endl;
                    std:: memset(recvBuf, '\0', DEFAULT_BUFLEN);
                    recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
                    //�յ��û���ִ
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
                    //����������ʼ��
                    //-----------------------------------------------------

                    //----------------------------------------------------
                    //---------------���ͺ���ģ��----------------------------
                    //------------------------------------------------------
                    HANDLE pipeline = CreateThread(NULL, 0, SendPipeline,
                                                   (void *)&cache, 0, NULL);
                    CloseHandle(pipeline);
                    //�߳�����
                    //-------------------------------------------

                    unsigned long curTransCount = 0;
                    while (curTransCount < checkRes.transCount)
                    {
                        while (cache.judgeEmpty() == NOT_EMPTY && curTransCount < checkRes.transCount)
                        //ֻҪ�ж����ͷ������
                        {
                            // cout << curTransCount << endl;
                            // cout << cache.header << " " << cache.end << " " << cache.round;
                            int iResult = send(ClientSocket, cache.cacheBuf[cache.end], CACHE_LENGTH, 0);
                            //�ڷ��Ͷ�socket�൱�ڻ�������������
                            if (cache.end == CACHE_WID_SEND - 1)
                            {
                                cache.round = 0;
                            }
                            cache.end = (cache.end + 1) % CACHE_WID_SEND;
                            //�޸Ķ�д��־λ
                            //------------------------------------------------------------------------

                            curTransCount++;
                            if (iResult == SOCKET_ERROR)
                            {
                                printf("�ڲ�ѭ��: %d ", WSAGetLastError());
                                std::cout << curTransCount << std::endl;
                                Sleep(5000);
                                // closesocket(ConnectSocket);
                                WSACleanup();
                                return 1;
                            }
                        }
                    }
                    //--------------------------------------------------------------------------
                    //-------------------------���������鴫��------------------------------------
                    //---------------------------------------------------------------------------
                    while (cache.judgeEmpty() == EMPTY)
                        ;
                    std::cout << "�������Ĵ���" <<std:: endl;
                    int iResult = send(ClientSocket, cache.cacheBuf[cache.end], cache.lastTimeSize, 0);
                    if (iResult == SOCKET_ERROR)
                    {
                        printf("���һ��ʧ��: %d\n", WSAGetLastError());
                        WSACleanup();
                        return 1;
                    }
                    std::cout << "���ͽ���" << std::endl;
                    //�������һƬ
                    //------------------------------------------------------------------
                }

                
                else
                //�������Ҫ�õ�ȷ�ϻ�ִ���ļ�
                {
                    cout << retVal << endl;
                    send(ClientSocket, retVal.c_str(), retVal.length(), 0);
                    //���û�����ȷ����Ϣ
                    std::memset(recvBuf, '\0', DEFAULT_BUFLEN);
                    recv(ClientSocket, recvBuf, DEFAULT_BUFLEN, 0);
                    //�յ��û���ִ
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
                        //����������ʼ��
                        //-----------------------------------------------------

                        //----------------------------------------------------
                        //---------------���ͺ���ģ��----------------------------
                        //------------------------------------------------------
                        HANDLE pipeline = CreateThread(NULL, 0, SendPipeline,
                                                       (void *)&cache, 0, NULL);
                        CloseHandle(pipeline);
                        //�߳�����
                        //-------------------------------------------

                        unsigned long curTransCount = 0;
                        while (curTransCount < checkRes.transCount)
                        {
                            while (cache.judgeEmpty() == NOT_EMPTY && curTransCount < checkRes.transCount)
                            //ֻҪ�ж����ͷ������
                            {
                                // cout << curTransCount << endl;
                                // cout << cache.header << " " << cache.end << " " << cache.round;
                                int iResult = send(ClientSocket, cache.cacheBuf[cache.end], CACHE_LENGTH, 0);
                                //�ڷ��Ͷ�socket�൱�ڻ�������������
                                if (cache.end == CACHE_WID_SEND - 1)
                                {
                                    cache.round = 0;
                                }
                                cache.end = (cache.end + 1) % CACHE_WID_SEND;
                                //�޸Ķ�д��־λ
                                //------------------------------------------------------------------------

                                curTransCount++;
                                if (iResult == SOCKET_ERROR)
                                {
                                    printf("�ڲ�ѭ��: %d ", WSAGetLastError());
                                    std::cout << curTransCount << std::endl;
                                    Sleep(5000);
                                    // closesocket(ConnectSocket);
                                    WSACleanup();
                                    return 1;
                                }
                            }
                        }
                        //--------------------------------------------------------------------------
                        //-------------------------���������鴫��------------------------------------
                        //---------------------------------------------------------------------------
                        while (cache.judgeEmpty() == EMPTY)
                            ;
                        std::cout << "�������Ĵ���" << std::endl;
                        int iResult = send(ClientSocket, cache.cacheBuf[cache.end], cache.lastTimeSize, 0);
                        if (iResult == SOCKET_ERROR)
                        {
                            printf("���һ��ʧ��: %d\n", WSAGetLastError());
                            WSACleanup();
                            return 1;
                        }
                        std::cout << "���ͽ���" << std::endl;
                        //�������һƬ
                        //------------------------------------------------------------------
                        //������ҵõ��û�ȷ��������ļ�������߳�
                    }
                }

                FileServer checkRes = inquiry->filelist->check(curMsg.sender);
            }
        }
        else if (res == 0)
            printf("���߳����Ӽ����ر�");
        else
        {
            printf("���߳����ӷ�������", WSAGetLastError());
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
       //ֻҪ�пվͷ������
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
    //------------------------����������д��-----------------------------
    //-----------------------------------------------------------------
    
    memset(cachePtr->cacheBuf[cachePtr->header], '\0', CACHE_LENGTH);
    //���㵱ǰ��
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
    //���һƬ��д��
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
        //�ʵ�����д���ļ����ٶ� Ϊ���ն��ṩ����ʱ�� ÿ�ζ��궼Ҫ�޸�end
        //ѭ������������������ �޸Ŀ�����־��
        //----------------------------------------------------
    }
    //-------------------���鴫������------------------------------
    
    while(cachePtr -> judgeEmpty() == EMPTY) ;//�ȴ����һƬд����
    fwrite(cachePtr->cacheBuf[cachePtr->end], cachePtr->lastTimeSize,
           1, cachePtr->fp);
    fclose(cachePtr->fp);
    cachePtr->pipeLineCond = DONE;
    std:: cout << "done save" << std:: endl;
    return 0;
}



#endif