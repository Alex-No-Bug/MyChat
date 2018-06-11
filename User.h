#pragma once

#ifndef USER_H  
#define USER_H  
/* 
    �û���Ķ���
    ���û��б���Ķ���
 */

#include "ConfigDefine.h"
#include "vector"

class User
{
    public:
        std::string userName = "";//�û���
        std::string cipher = "";//����
        std::vector<User> friend_1;
        int onlineStatu = OFFLINE;//�û�����״̬ �궨��ONLINE OFFLINE
};



class UserList
{
    public:
        std::vector<User> userList;
    
    void init()
    //��ʼ�������б�
    {
        userList.clear();
        User user1;
        User user2;
        User user3;
        User user4;
        User user5;
        User user6;
        User user7;
        User user8;
        User user9;

        userList.push_back(user1);
        userList.push_back(user2);
        userList.push_back(user3);
        userList.push_back(user4);
        userList.push_back(user5);
        userList.push_back(user6);
        userList.push_back(user7);
        userList.push_back(user8);
        userList.push_back(user9);

        for (int i = 0; i <= 8; i++)
        {
            userList[i].userName = i + 1 + '0'; //��ʼ������
            for (int j = 1; j <= 6; j++)
                userList[i].cipher.push_back('0' + 1 + i); //����6��1

            userList[i].onlineStatu = OFFLINE;
        }
        for (int i = 0; i <= 8; i++)
        {
            if (i == 0) //1��2-9���Ǻ���
            {
                for (int k = 1; k <= 8; k++)
                {
                    userList[i].friend_1.push_back(userList[k]);
                    userList[k].friend_1.push_back(userList[i]);
                }
            }
            else if (i == 1) //2�ı���
            {
                for (int k = 2; k <= 8; k++)
                {
                    if ((k + 1) % 2 == 0) //��2��������
                    {
                        userList[i].friend_1.push_back(userList[k]);
                        userList[k].friend_1.push_back(userList[i]);
                    }
                }
            }
            else if (i == 2) //3�ı���
            {
                for (int k = 3; k <= 8; k++)
                {
                    if ((k + 1) % 3 == 0) //3�ı���
                    {
                        userList[i].friend_1.push_back(userList[k]);
                        userList[k].friend_1.push_back(userList[i]);
                    }
                }
            }
        }
    }


    int checkLogin(std:: string userName, std:: string cipher)
    //��ȷ�򷵻غ궨��SUCCESS���޸ĸ��û���onlineStatu��ΪONLINE
    //���򷵻غ궨��FAIL�������û���onlineStatu��ΪOFFLINE
	{
		for (int index = 0; index <= 8; index++)
		{
			if (userName.compare(userList[index].userName) == 0)//name matching
			{
				if (cipher.compare(userList[index].cipher) == 0)//passs_word matching
				{
					userList[index].onlineStatu = ONLINE;
					return(SUCCESS);
				}
				else
				{
					userList[index].onlineStatu = OFFLINE;
					return(FAIL);//ʧ�� password ERROR
				}
			}
		}
		return (FAIL);// no such a person in the table
	}


    std:: string checkOnlineFriend(std:: string userName)
    //��ѯ��ǰ���ߺ��� �����غ��ѵ����ֵ��ַ���
    //��ʽΪ
    // |XXXX|YYYYY|ZZZZZ|.....
    //  ����1 ����2 ����3  ��|�ָ�
    //  �����ǰ�����ߺ����򷵻�һ�� "|"
    {
        std::string friend_name = "|";
        for (int index = 0; index <= 8; index++)
        {
            if (userList[index].userName.compare(userName) == 0) //find the person
            {
                //go through the list of his friends.
                int i;
                for (i = 0; i < userList[index].friend_1.size(); i++)
                {
                    if (userList[index].friend_1[i].onlineStatu == ONLINE) // return online friend
                    {
                        friend_name = friend_name + userList[index].friend_1[i].userName + '|';
                    }
                }
                return (friend_name);
            }
        }
    }

    std:: string getFri(std:: string userName)
    //��ѯ��ǰ�û����к���
    //�����ַ����ĸ�ʽͬ��
	{
		std::string friend_name="|";
		for (int index = 0; index <= 8; index++)
		{
			if (userList[index].userName.compare(userName) == 0)//find the person
			{
				//go through the list of his friends.
				int i;
				for (i = 0; i < userList[index].friend_1.size(); i++)
				{
					friend_name = friend_name + userList[index].friend_1[i].userName + '|';
				}
				return(friend_name);
			}
		}
	}

};



#endif 
