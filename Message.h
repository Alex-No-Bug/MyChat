#pragma once

#ifndef MESSAGE_H
#define MESSAGE_H
/* Message�����Ϣ�����б� */

#include "ConfigDefine.h"
#include "vector"

//��¼���ĸ�ʽ LOGIN_INFO|sender|cipher|
//ע����¼���ĸ�ʽ OFFLINE_INFO|sender|"OK"|
//��ͨ���ĸ�ʽ SHROT_MSG|sender|receiver|message
//��ѯ���ĸ�ʽ INQ_MSG|sender|"OK"|
//�ļ�ȷ�ϱ��ĸ�ʽ ���û� -> ������ ��F_C_M|sender|receiver|file_info
//file_info��ʽ transConut|lastTimeSize|fileName|FILE_MSG��GRAPH_MSG
//�ļ�ȷ�ϱ��ĸ�ʽ (������->�û�) F_C_M|sender|fileName|file_info
//file_info��ʽ transCount|lastTimeSize|FILE_MSG��GRAPH_MSG

class Message
{
  public:
    char msgFlag = 0;    //�������� ���궨��
    string sender = "";   //������
    string receiver = ""; //������
    string message = "";  //��������

    Message()
    {
        ;
    }

    Message(char flag, string send, string recv, string msg)
    //���캯��
    {
        msgFlag = flag;
        sender = send;
        receiver = recv;
        message = msg;
    }

    Message(string rawMeg)
    //����һ�����ϱ�׼��ʽ�ı���
    //��ʽΪ X|AAAA|BBBBB|CCCCCCCC
    //�Դ�Ϊ��������һ��Message��
    {
        int index = 0;
        msgFlag = rawMeg[index];
        index++; //��ʱ=|
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
    //��һ��Message�������д�뵽һ��string�� �������ɿͻ��˸��������ķ��ͱ���
    //��ʽΪ X|AAAA|BBBBB|CCCCCCCC
    //XΪ�������� AAAAΪsender BBBBΪreceiver CCCCΪ��������
    //�м���"|"�ָ�
    //AAAA��BBBB��Ϊ�������ַ��� Xֻռһ���ַ�
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
    unsigned int size = 0;        //��ǰvector�����ݸ�ʽ
    vector<Message> mesCache; //��Ϣ�����

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
//����Ƿ��и��û�����Ϣ����
//��ʽΪ��'*' ������ ���� ��Ϣ ����
//ÿ��ȡ��һ����Ϣ �ͽ�����Ϣ�����ݿ���ɾ��
{
    string retVal = "*";
    if (size == 0)
        return retVal;
    //�����Ϣ����Ϊ�� ֱ�ӷ���һ��*
    //-----------------------------------------------------------

    for (int i = 0; i < size; i++)
    {
        if (mesCache.at(i).receiver == userName)
        {
            retVal += mesCache.at(i).sender + "*" + mesCache.at(i).message + "*";
            vector<Message>::iterator cur = mesCache.begin();
            mesCache.erase(cur + i);
            size--;
            //��װ����ɾ��һ����Ϣʱ��СҲҪͬ������
            i -= 1;
        }
    }
    return retVal;
}

#endif