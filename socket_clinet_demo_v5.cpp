/* 客户端版本5
    udp文件传输demo
    你要新建一种报文类型
    暂时还使用27015来传文件*/

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
//老版本中用于收发一体的端口
//在v3版本中只用于服务器的收和客户端的发
#define INQUIRY_PORT "27016"
//v3多线程版本中的子端口
//用于服务器的发和客户端的收
#define FILE_PORT 27030
//v5版本中用于UDP收发文件的子端口

int __cdecl main()
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    char sendbuf[DEFAULT_BUFLEN] = {0};
    char recvbuf[DEFAULT_BUFLEN] = {0};
    int funcRes;
    int recvbufLen = DEFAULT_BUFLEN;

    cout << "请输入服务器当前IP地址" << endl;
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
    //从服务器端返回的地址协议是非指定的
    //AF_INET指定ipv4
    //AF_INET6指定ipv6
    //在UDP中是AF_INET 在之前的版本中是非指定方式
    hints.ai_socktype = SOCK_DGRAM;//TCP里STREAM模式
    hints.ai_protocol = IPPROTO_UDP;
    //TCP协议中该参数是IPPROTO_TCP
    //配置SOCKET基本参数

    string input;
    input.clear();
    //用户输入字符串
    
    int userName = 0;
    string userip="";

    // do
    // {
    //     //1name|code|ip
    //     getLoginInfo(&input);
    //     //切割 
    //     int i=1;
    //     string temp="";
    //     while(input[i]!='|')
    //     {
    //         temp+=input[i++];
    //     }
    //     userName=atoi(temp.c_str());
    //     i++;//此时 |
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
    //用户身份验证完成

    //生成一个询问用户类对象来接收信息
    InqClitInfo clinetInfo(userName, ipAddrServer);
    //用该对象来初始化一个问询线程
    //子线程加在用户身份验证完成后和发送第一条消息报文前
    //HANDLE inquiryThread = CreateThread(NULL, 0, inquiry, (void*)&clinetInfo, 0, NULL);


    while (input != "2  ")
    //发送部分
    {

        input.clear();

        string userNameStr = to_string(userName);

        // fakeListen(&ipAddrServer, userNameStr, &hints, &result);
        string temp = "";
        int recvFri = 0;

        int fileOrNot = 0;
        cout << "需要发送文件到服务器吗？" << endl;
        cin >> fileOrNot;

        if(fileOrNot)
        //发送文件
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
            //在发送正式的文件内容之前 先发送要进行几次文件传送
            //传统的文件读写方法
            if(transCount == 0)
            //如果一次发送就搞定
            {
                sendMessageToServer(&ipAddrServer, to_string(fileSize), &hints, &result);
                sendFileToServer(&ipAddrServer, fileInMem, &hints, &result, fileSize, ConnectSocket);
            }
            else
            //大文件
            {
                for (unsigned long i = 0; i < transCount; i++)
                {
                    fread(fileInMem, DEFAULT_BUFLEN, 1, fp);
                    sendFileToServer(&ipAddrServer, fileInMem, &hints, &result, fileSize, ConnectSocket);
                    if(i % 400 == 0)
                        cout << "发送进度: " << float(i) / (float)(transCount) * 100 << endl;
                }
                memset(fileInMem, '\0', DEFAULT_BUFLEN);
                //缓冲区清零
                int leftSize = fileSize - DEFAULT_BUFLEN * transCount;
                fread(fileInMem, leftSize, 1, fp);
                string tempLeftSize = to_string(leftSize);
                char mark[5] = {0};
                for(int i = 0; i < tempLeftSize.size(); i ++) 
                    mark[i] = tempLeftSize[i];
                cout << "transCount: " << transCount << endl;
                cout <<"leftsize: " << leftSize << endl;
                sendFileToServer(&ipAddrServer, mark, &hints, &result, to_string(leftSize).size(), ConnectSocket);
                //传送最后一次发送的分包大小
                cout << "扔完了最后一次分包大小了" << endl;        
                sendFileToServer(&ipAddrServer, fileInMem, &hints, &result, leftSize, ConnectSocket);
                //最后一次分包
                fclose(fp);
            }
        
        }


        cout << "请输入发送对象" << endl;
        cin >> recvFri;
        getline(cin, temp);

        input += "2";
        input =input + to_string(userName)+"|";
        //获取时间
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

