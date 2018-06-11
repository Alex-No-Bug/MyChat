#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H
/* Message类和消息缓存列表 */

#include "ConfigDefine.h"
#include "vector"

//登录报文格式 LOGIN_INFO|sender|cipher|
//注销登录报文格式 OFFLINE_INFO|sender|"OK"|
//普通报文格式 SHROT_MSG|sender|receiver|message
//问询报文格式 INQ_MSG|sender|"OK"|
//文件确认报文格式 （用户 -> 服务器 ）F_C_M|sender|receiver|file_info
//file_info格式 transConut|lastTimeSize|fileName|FILE_MSG或GRAPH_MSG
//文件确认报文格式 (服务器->用户) F_C_M|sender|fileName|file_info
//file_info格式 transCount|lastTimeSize|FILE_MSG或GRAPH_MSG

class Message
{
  public:
    char msgFlag = 0;    //报文种类 见宏定义
    string sender = "";   //发送者
    string receiver = ""; //接受者
    string message = "";  //报文内容

    Message()
    {
        ;
    }

    Message(char flag, string send, string recv, string msg)
    //构造函数
    {
        msgFlag = flag;
        sender = send;
        receiver = recv;
        message = msg;
    }

    Message(string rawMeg)
    //传入一个符合标准格式的报文
    //格式为 X|AAAA|BBBBB|CCCCCCCC
    //以此为基础生成一个Message类
    {
        int index = 0;
        msgFlag = rawMeg[index];
        index++; //此时=|
        index++;
        while (rawMeg[index] != '|')
        {
            sender = sender + rawMeg[index];
            index++;
        }
        index++;
        while (rawMeg[index] != '|')
        {
            receiver = receiver + rawMeg[index];
            index++;
        }
        index++;
        while (rawMeg[index] != '\0')
        {
            message = message + rawMeg[index];
            index++;
        }
    }

    string toClientStr()
    //把一个Message类的数据写入到一个string中 用于生成客户端给服务器的发送报文
    //格式为 X|AAAA|BBBBB|CCCCCCCC
    //X为报文种类 AAAA为sender BBBB为receiver CCCC为报文内容
    //中间用"|"分隔
    //AAAA和BBBB都为不定长字符串 X只占一个字符
    {
        string s = "";
        s.push_back(msgFlag);
        s = s + "|" + sender + "|" + receiver + "|" + message;
        return (s);
    }
};

class MesCache
{
  public:
    unsigned int size = 0;        //当前vector的数据格式
    vector<Message> mesCache; //消息缓冲池

    MesCache()
    {
        mesCache.clear();
    }

    void push_cache(Message msg);

    string checkCache(string userName);
};

void MesCache::push_cache(Message msg)
{
    mesCache.push_back(msg);
    size++;
}

string MesCache::checkCache(string userName)
//检查是否有改用户的消息缓存
//格式为：'*' 发送者 换行 消息 换行
//每提取出一条消息 就将该消息在数据库中删除
{
    string retVal = "*";
    if (size == 0)
        return retVal;
    //如果消息队列为空 直接返回一个*
    //-----------------------------------------------------------

    for (int i = 0; i < size; i++)
    {
        if (mesCache.at(i).receiver == userName)
        {
            retVal += mesCache.at(i).sender + "*" + mesCache.at(i).message + "*";
            vector<Message>::iterator cur = mesCache.begin();
            mesCache.erase(cur + i);
            size--;
            //封装类中删除一条消息时大小也要同步更新
            i -= 1;
        }
    }
    return retVal;
}

#endif