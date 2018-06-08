/* �ͻ��˰汾5
    udp�ļ�����demo
    ��Ҫ�½�һ�ֱ�������
    ��ʱ��ʹ��27015�����ļ�*/

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "string"
#include "iostream"
#include "client_user.h"
#include "time.h"
#include "fstream"
using namespace std;


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512

#define DEFAULT_PORT "27015"
//�ϰ汾�������շ�һ��Ķ˿�
//��v3�汾��ֻ���ڷ��������պͿͻ��˵ķ�
#define INQUIRY_PORT "27016"
//v3���̰߳汾�е��Ӷ˿�
//���ڷ������ķ��Ϳͻ��˵���
#define FILE_PORT 27030
//v5�汾������UDP�շ��ļ����Ӷ˿�

int __cdecl main()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char sendbuf[DEFAULT_BUFLEN] = {0};
    char recvbuf[DEFAULT_BUFLEN] = {0};
    int funcRes;
    int recvbufLen = DEFAULT_BUFLEN;

    cout << "�������������ǰIP��ַ" << endl;
    string ipAddrServer;
    getline(cin, ipAddrServer);
    system("pause");
    system("cls");

    funcRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (funcRes != 0) {
        printf("WSAStartup failed with error: %d\n", funcRes);
        return 1;
    }
    // ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_INET;
    //�ӷ������˷��صĵ�ַЭ���Ƿ�ָ����
    //AF_INETָ��ipv4
    //AF_INET6ָ��ipv6
    //��UDP����AF_INET ��֮ǰ�İ汾���Ƿ�ָ����ʽ
    hints.ai_socktype = SOCK_DGRAM;//TCP��STREAMģʽ
    hints.ai_protocol = IPPROTO_UDP;
    //TCPЭ���иò�����IPPROTO_TCP
    //����SOCKET��������

    string input;
    input.clear();
    //�û������ַ���
    
    int userName = 0;
    string userip="";

    // do
    // {
    //     //1name|code|ip
    //     getLoginInfo(&input);
    //     //�и� 
    //     int i=1;
    //     string temp="";
    //     while(input[i]!='|')
    //     {
    //         temp+=input[i++];
    //     }
    //     userName=atoi(temp.c_str());
    //     i++;//��ʱ |
    //     while(input[i]!='|')
    //     {
    //        i++;
    //     }
    //     i++;
      
    //     while(input[i]!= 0)
    //     {
    //         userip+=input[i++];
    //     }
    //     funcRes = firstTimeLogin(&ipAddrServer, input, &hints, &result);
    // } while(funcRes != 0);
    //�û�������֤���

    //����һ��ѯ���û��������������Ϣ
    InqClitInfo clinetInfo(userName, ipAddrServer);
    //�øö�������ʼ��һ����ѯ�߳�
    //���̼߳����û�������֤��ɺ�ͷ��͵�һ����Ϣ����ǰ
    //HANDLE inquiryThread = CreateThread(NULL, 0, inquiry, (void*)&clinetInfo, 0, NULL);


    while (input != "2  ")
    //���Ͳ���
    {

        input.clear();

        string userNameStr = to_string(userName);

        // fakeListen(&ipAddrServer, userNameStr, &hints, &result);
        string temp = "";
        int recvFri = 0;

        int fileOrNot = 0;
        cout << "��Ҫ�����ļ�����������" << endl;
        cin >> fileOrNot;

        if(fileOrNot)
        //�����ļ�
        {
            char fileName[100] = "C:\\Users\\Mark.Wen\\Desktop\\SupremeChat\\ConsoleApplication2\\Debug\\1.ppt";
            unsigned long fileSize = 0;

            FILE* fp = fopen(fileName, "rb");
            fseek(fp, 0, SEEK_END);
            fileSize = ftell(fp);
            rewind(fp);
            char* fileInMem = new char [fileSize];
            memset(fileInMem, 0, fileSize);
            unsigned long transCount = fileSize / DEFAULT_BUFLEN;
            sendMessageToServer(&ipAddrServer, to_string(transCount), &hints, &result);
            //�ڷ�����ʽ���ļ�����֮ǰ �ȷ���Ҫ���м����ļ�����
            //��ͳ���ļ���д����
            if(transCount == 0)
            //���һ�η��;͸㶨
            {
                sendMessageToServer(&ipAddrServer, to_string(fileSize), &hints, &result);
                sendFileToServer(&ipAddrServer, fileInMem, &hints, &result, fileSize, ConnectSocket);
            }
            else
            //���ļ�
            {
                for (unsigned long i = 0; i < transCount; i++)
                {
                    fread(fileInMem, DEFAULT_BUFLEN, 1, fp);
                    sendFileToServer(&ipAddrServer, fileInMem, &hints, &result, fileSize, ConnectSocket);
                    if(i % 400 == 0)
                        cout << "���ͽ���: " << float(i) / (float)(transCount) * 100 << endl;
                }
                memset(fileInMem, '\0', DEFAULT_BUFLEN);
                //����������
                int leftSize = fileSize - DEFAULT_BUFLEN * transCount;
                fread(fileInMem, leftSize, 1, fp);
                string tempLeftSize = to_string(leftSize);
                char mark[5] = {0};
                for(int i = 0; i < tempLeftSize.size(); i ++) 
                    mark[i] = tempLeftSize[i];
                cout << "transCount: " << transCount << endl;
                cout <<"leftsize: " << leftSize << endl;
                sendFileToServer(&ipAddrServer, mark, &hints, &result, to_string(leftSize).size(), ConnectSocket);
                //�������һ�η��͵ķְ���С
                cout << "���������һ�ηְ���С��" << endl;        
                sendFileToServer(&ipAddrServer, fileInMem, &hints, &result, leftSize, ConnectSocket);
                //���һ�ηְ�
                fclose(fp);
            }
        
        }


        cout << "�����뷢�Ͷ���" << endl;
        cin >> recvFri;
        getline(cin, temp);

        input += "2";
        input =input + to_string(userName)+"|";
        //��ȡʱ��
        time_t timep;
        time(&timep);
        char tmp[64];
        strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
        input = input + to_string(recvFri) + "|" + tmp + '|';
        input += temp;

        funcRes = sendMessageToServer(&ipAddrServer, input, &hints, &result);
    }
    system("pause");
}
