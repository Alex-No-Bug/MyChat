#pragma once

#ifndef CONFIGDEFINE_H  
#define CONFIGDEFINE_H   
/* 服务器端的宏定义 
    同时去掉了std的命名空间*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
using namespace std;




#define DEFAULT_BUFLEN 8800
#define CACHE_LENGTH DEFAULT_BUFLEN
#define DEFAULT_PORT "27015"
#define INQUIRY_PORT "27016"
#define FILE_PORT "27030"
#define GRAPH_PORT "27041"
#define CACHE_WID_RECV 100
//服务器端既需要收文件
#define CACHE_WID_SEND 42
//服务器端也需要发文件
//端口号和空间大小的宏定义

#define LOCK 0
#define UNLOCK 1
#define NOT_EMPTY 0
#define EMPTY 1
#define NOT_FULL 0
#define FULL 1
#define SUCCESS 1
#define FAIL 0
#define DONE 0
#define NOT_DONE 1
//多线程控制量的宏定义

#define ONLINE 1
#define OFFLINE 0
//User.h里在线状态的宏定义

#define LOGIN_INFO '0'
#define OFFLINE_INFO '1'
#define SHORT_MSG '2'
#define INQ_MSG '3'
#define FILE_MSG '4'
#define GRAPH_MSG '5'
#define FILE_CHECK_MSG '6' //用户确认接收文件还是拒绝接收文件的的报文
//报文种类宏定义

#define YES '0'
#define NO '1'
//用户确认文件发送的宏定义

#define MSG_FROM_INQ '*'
#define FRI_FROM_INQ '|'
//区别监听线程反馈消息种类

//登录报文格式 LOGIN_INFO|sender|cipher|XXX
//注销登录报文格式 OFFLINE_INFO|sender|"OK"|
//普通报文格式 SHROT_MSG|sender|receiver|message
//问询报文格式 INQ_MSG|sender|"OK"|
//文件确认报文格式 （用户 -> 服务器 ）FILE_MSG|sender|receiver|file_info
//file_info格式 transConut|lastTimeSize|fileName|FILE_MSG或GRAPH_MSG
//文件确认报文格式 (服务器->用户) F_C_M|sender|fileName|file_info
//file_info格式 transCount|lastTimeSize|FILE_MSG或GRAPH_MSG 

/* 文件发送的流程（服务器->客户）
    服务器发送文件确认报文
    等待用户回信
    用户回复#define NO 则抛弃 否则发送 */

/* 文件发送的流程（客户->服务器）
    客户端发送文件确认报文
    等待服务器回信"OK"
    接收回信之后直接发送 */

/* 登录反馈的报文：
    首先发一个"OK"
    接着发送所有好友的名单
    若登录失败则发送一个"FAIL" */

/* 所有好友名单的格式
    |XXXX|YYYYY|ZZZZZ|.....|
    |好友1| 好友2 |好友3|...|  用|分隔
     */

/* 短消息报文的反馈
    若正确收到则给客户端发送"OK"
    否则发送"FAIL" */


#endif 