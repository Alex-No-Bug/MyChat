/* �������˵��ļ���Ϣ */
#pragma once
#include "ConfigDefine.h"
#include "Message.h"

class FileServer
{
public:
    unsigned long fileSize = 0;
    unsigned long transCount = 0;
    unsigned long lastTimeSize = 0;
    char fileOrGra = 0;//������
    std:: string fileName = "";
    std:: string path = "C:\\Users\\Mark.Wen\\Desktop\\SupremeChat\\";
    std:: string sender = "";
    std:: string receiver = "";

    FileServer(Message fileCheckMsg)
    //�ͻ��������������ļ�ȷ�ϱ���
    //�ļ�ȷ�ϱ��ĸ�ʽ ���û� -> ������ ��F_C_M|sender|receiver|file_info
     //file_info��ʽ transConut|lastTimeSize|fileName|FILE_MSG��GRAPH_MSG
    {
        sender = fileCheckMsg.sender;
        receiver = fileCheckMsg.receiver;
        //transcount
        int index = 0;
        std::string number = "";
        while (fileCheckMsg.message[index] != '|')
        {
            number = number + fileCheckMsg.message[index];
            index++;
        }
        transCount = atol(number.c_str());

        //Lastimesize index=|
        index++;
        number = "";
        while (fileCheckMsg.message[index] != '|')
        {
            number = number + fileCheckMsg.message[index];
            index++;
        }
        lastTimeSize = atol(number.c_str());
        //fileName
        index++;
        number = "";
        while (fileCheckMsg.message[index] != '|')
        {
            number = number + fileCheckMsg.message[index];
            index++;
        }
        fileName = number;

        //index==|
        index++;
        fileOrGra = fileCheckMsg.message[index];
        fileSize = transCount * CACHE_LENGTH + lastTimeSize;
    }
    //����һ���ļ�ȷ�ϱ���


    FileServer(int a){;}

    std:: string toString()
    //�������¸�ʽ���ַ���
    //�ļ�ȷ�ϱ��ĸ�ʽ (������->�û�) F_C_M|sender|receiver|fileName|file_info
    //file_info��ʽ transCount|lastTimeSize|flagOrGra 
    //FILE_CHECK_MSG
    {
        std::string message = "";
        message.push_back(FILE_CHECK_MSG);
        message = "|" + message + sender + "|" + receiver + "|"
         + fileName + "|" + std::to_string(transCount) + "|" + 
        std::to_string(lastTimeSize) + "|";
        message.push_back(fileOrGra);
        
        return message;
    }

};

class FileList 
{
public:
    unsigned int size = 0;
    std:: vector<FileServer> fileCache;

    FileList()
    {
        fileCache.clear();
    }

    void push_cache(FileServer f)
    {
        fileCache.push_back(f);
        size++;
    }

    FileServer check(std::string userName)
    //Ĭ�ϵ�ȱʡ·����EMPTY
    //����һ���û���
    //����һ���ļ�����Ϣ����
    //�����ǰû�и��û����ļ� �򷵻ؿ������
    {
        FileServer tempF(1);
        tempF.path = "EMPTY";
        if (size == 0)
            return tempF;
        //�����Ϣ����Ϊ�� ֱ�ӷ���һ���յ�FileServer
        //--------------------------------------------------------------

        for (int i = 0; i < size; i++)
        {
            if (fileCache.at(i).receiver == userName)
            {
                tempF = fileCache.at(i);
                //��ֵ����ȥ
                std::vector<FileServer>::iterator cur = fileCache.begin();
                fileCache.erase(cur + i);
                size--;
                //��װ����ɾ��һ����Ϣʱ��СҲҪͬ������
                break;
                //ֻ��һ���ļ� �ҵ����˳�
            }
        }
        return tempF;
    }
    
    //���û������ļ�ʱ Ҫ��ѯ���Ƿ�׼������
    //����Ը����� ������ǰ�ļ� ����������ļ����� ��ѯ��ʣ�µ��ļ�
    //ֱ��û�е�ǰ�û����ļ�

};
