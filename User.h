#pragma once

#ifndef USER_H  
#define USER_H  
/* 
    用户类的定义
    和用户列表类的定义
 */

#include "ConfigDefine.h"
#include "vector"

class User
{
    public:
        std::string userName = "";//用户名
        std::string cipher = "";//密码
        std::vector<User> friend_1;
        int onlineStatu = OFFLINE;//用户在线状态 宏定义ONLINE OFFLINE
};



class UserList
{
    public:
        std::vector<User> userList;
    
    void init()
    //初始化整个列表
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
            userList[i].userName = i + 1 + '0'; //初始化名字
            for (int j = 1; j <= 6; j++)
                userList[i].cipher.push_back('0' + 1 + i); //密码6个1

            userList[i].onlineStatu = OFFLINE;
        }
        for (int i = 0; i <= 8; i++)
        {
            if (i == 0) //1和2-9都是好友
            {
                for (int k = 1; k <= 8; k++)
                {
                    userList[i].friend_1.push_back(userList[k]);
                    userList[k].friend_1.push_back(userList[i]);
                }
            }
            else if (i == 1) //2的倍数
            {
                for (int k = 2; k <= 8; k++)
                {
                    if ((k + 1) % 2 == 0) //是2的整数倍
                    {
                        userList[i].friend_1.push_back(userList[k]);
                        userList[k].friend_1.push_back(userList[i]);
                    }
                }
            }
            else if (i == 2) //3的倍数
            {
                for (int k = 3; k <= 8; k++)
                {
                    if ((k + 1) % 3 == 0) //3的倍数
                    {
                        userList[i].friend_1.push_back(userList[k]);
                        userList[k].friend_1.push_back(userList[i]);
                    }
                }
            }
        }
    }


    int checkLogin(std:: string userName, std:: string cipher)
    //正确则返回宏定义SUCCESS并修改该用户的onlineStatu设为ONLINE
    //否则返回宏定义FAIL并将该用户的onlineStatu设为OFFLINE
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
					return(FAIL);//失败 password ERROR
				}
			}
		}
		return (FAIL);// no such a person in the table
	}


    std:: string checkOnlineFriend(std:: string userName)
    //查询当前在线好友 并返回好友的名字的字符串
    //格式为
    // |XXXX|YYYYY|ZZZZZ|.....
    //  好友1 好友2 好友3  用|分隔
    //  如果当前无在线好友则返回一个 "|"
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
    //查询当前用户所有好友
    //返回字符串的格式同上
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
