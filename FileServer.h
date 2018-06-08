/* �������˵��ļ���Ϣ */
#pragma once
#include "ConfigDefine.h"
#include "Message.h"
#include <string>     
#include <cstdlib>//�������
class FileServer
{
public:
    unsigned long fileSize;
    unsigned long transCount;
    unsigned long lastTimeSize;
    char fileOrGra;//������
    std:: string fileName;
    std:: string path;
    std:: string sender;
    std:: string receiver;

    FileServer(Message fileCheckMsg)
    //�ͻ��������������ļ�ȷ�ϱ���
    //�ļ�ȷ�ϱ��ĸ�ʽ ���û� -> ������ ��F_C_M|sender|receiver|file_info
     //file_info��ʽ transConut|lastTimeSize|fileName|FILE_MSG��GRAPH_MSG
    {
        sender=fileCheckMsg.sender;
        receiver=fileCheckMsg.receiver;
       //transcount
        int index=0;
        std::string number="";
        while(fileCheckMsg.message[index]!='|')
        {
            number=number+fileCheckMsg.message[index];
            index++;
        }
         transCount=atol(number.c_str());

         //Lastimesize index=|
         index++;
         number="";
        while(fileCheckMsg.message[index]!='|')
        {
            number=number+fileCheckMsg.message[index];
            index++;
        }
          lastTimeSize =atol(number.c_str());
          //fileName
          index++;
          number="";
          while(fileCheckMsg.message[index]!='|')
          {
            number=number+fileCheckMsg.message[index];
            index++;
          }
          fileName=number;

          //index==|
          index++;
          fileOrGra=fileCheckMsg.message[index];
    }
    //����һ���ļ�ȷ�ϱ���
    FileServer(int a){;}

    std:: string toString()
    //�����������ͻ����ļ�ȷ�ϱ���
   {
  //�ļ�ȷ�ϱ��ĸ�ʽ (������->�û�) F_C_M|sender|fileName|file_info
   //file_info��ʽ transCount|lastTimeSize|FILE_MSG��GRAPH_MSG 
    std::string message="";
    message.push_back(FILE_CHECK_MSG);
    message=message+sender+receiver+fileName+"|"+std::to_string(transCount)+"|"+std::to_string(lastTimeSize)+"|";
    message.push_back(fileOrGra);
   }
};

class FileList 
{
public:
    unsigned int size;
    std:: vector<FileServer> fileCache;

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
