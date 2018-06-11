#pragma once

#ifndef CONFIGDEFINE_H  
#define CONFIGDEFINE_H   
/* �������˵ĺ궨�� 
    ͬʱȥ����std�������ռ�*/

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
//�������˼���Ҫ���ļ�
#define CACHE_WID_SEND 42
//��������Ҳ��Ҫ���ļ�
//�˿ںźͿռ��С�ĺ궨��

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
//���߳̿������ĺ궨��

#define ONLINE 1
#define OFFLINE 0
//User.h������״̬�ĺ궨��

#define LOGIN_INFO '0'
#define OFFLINE_INFO '1'
#define SHORT_MSG '2'
#define INQ_MSG '3'
#define FILE_MSG '4'
#define GRAPH_MSG '5'
#define FILE_CHECK_MSG '6' //�û�ȷ�Ͻ����ļ����Ǿܾ������ļ��ĵı���
//��������궨��

#define YES '0'
#define NO '1'
//�û�ȷ���ļ����͵ĺ궨��

#define MSG_FROM_INQ '*'
#define FRI_FROM_INQ '|'
//��������̷߳�����Ϣ����

//��¼���ĸ�ʽ LOGIN_INFO|sender|cipher|XXX
//ע����¼���ĸ�ʽ OFFLINE_INFO|sender|"OK"|
//��ͨ���ĸ�ʽ SHROT_MSG|sender|receiver|message
//��ѯ���ĸ�ʽ INQ_MSG|sender|"OK"|
//�ļ�ȷ�ϱ��ĸ�ʽ ���û� -> ������ ��FILE_MSG|sender|receiver|file_info
//file_info��ʽ transConut|lastTimeSize|fileName|FILE_MSG��GRAPH_MSG
//�ļ�ȷ�ϱ��ĸ�ʽ (������->�û�) F_C_M|sender|fileName|file_info
//file_info��ʽ transCount|lastTimeSize|FILE_MSG��GRAPH_MSG 

/* �ļ����͵����̣�������->�ͻ���
    �����������ļ�ȷ�ϱ���
    �ȴ��û�����
    �û��ظ�#define NO ������ ������ */

/* �ļ����͵����̣��ͻ�->��������
    �ͻ��˷����ļ�ȷ�ϱ���
    �ȴ�����������"OK"
    ���ջ���֮��ֱ�ӷ��� */

/* ��¼�����ı��ģ�
    ���ȷ�һ��"OK"
    ���ŷ������к��ѵ�����
    ����¼ʧ������һ��"FAIL" */

/* ���к��������ĸ�ʽ
    |XXXX|YYYYY|ZZZZZ|.....|
    |����1| ����2 |����3|...|  ��|�ָ�
     */

/* ����Ϣ���ĵķ���
    ����ȷ�յ�����ͻ��˷���"OK"
    ������"FAIL" */


#endif 