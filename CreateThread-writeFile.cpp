// CreateThread-writeFile.cpp : 定义控制台应用程序的入口点。
#include "stdafx.h"
#include <stdlib.h>
#include <iostream>
#include <list>
#include <conio.h>
#include <time.h>
#include <algorithm>
#include <windows.h>
#include <fstream>


using namespace std;
#define Patha "E://demo.txt"
#define Pathb "E://b.txt"
#define SIZE 256 //5kB较大的缓冲区
#define N 6
char cache[N][SIZE];
int read = 0; //第一个缓冲区的头指针
			  //定义信号量 1 可以从a.txt文件读 0 不可以

//创建一个信号量
HANDLE hSemaphore;

class Cache
{
  public:
	char mem_cache[N][SIZE];
	int size;
	// int head;
	// int end;
	int sem; //信号量
	FILE *fp;
	int num; //大循环的次数
	int fileSize;
	Cache(char a[N][SIZE], int s, char *str)
	{
		for (int i = 0; i < N; i++)
			for (int j = 0; j < SIZE; j++)
				mem_cache[i][j] = a[i][j];
		// head = Head;
		// end = End;
		sem = s;
		//文件指针需要传入 共享
		fp = fopen(str, "rb");
		fseek(fp, 0, SEEK_END); //定位文件的头
		fileSize = ftell(fp);   //当前指针到File结尾的长度
		cout << "文件大小：" << fileSize << endl;
		rewind(fp);
		num = (fileSize / (N * SIZE)) + 1;
		cout << "循环" << num << "次" << endl;
	}
};

//读文件的线程
DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{

	Cache *p = (Cache *)lpParameter;
	if (p->mem_cache[0][0] == '\0')
		p->sem = 1;
	for (int k = 0; k < p->num; k++)
	{
		WaitForSingleObject(hSemaphore, INFINITE);
		cout << "还需" << p->num << "   - " << k << "循环" << endl;
		//if (p->sem == 1)
		//{
		//cout << "p->fileSIZE:  " << p->fileSize << "NSIZE  " << N*SIZE << endl;
		if (p->fileSize > N * SIZE) //如果大于读取一个缓冲区的量
		{
			for (int j = 0; j < N; j++) //循环读入
				fread(p->mem_cache[j], SIZE, 1, p->fp);

			cout << "输出第一次读入的" << endl;
			for (int i = 0; i < N; i++)
				for (int j = 0; j < SIZE; j++)
					cout << p->mem_cache[i][j] << endl; //输出

			fclose(p->fp);
			Sleep(2000);
			ReleaseSemaphore(hSemaphore, 1, NULL); //释放信号量
												   //p->sem = 0;
		}
		else
		{
			int leftSize = N * SIZE - p->fileSize;
			int N_1 = leftSize / SIZE;			 //商
			int real_leftSize = leftSize % SIZE; //余数 最后零头
			for (int i = 0; i < N_1; i++)
				fread(p->mem_cache[i], SIZE, 1, p->fp);
			fread(p->mem_cache[N_1], real_leftSize, 1, p->fp);

			for (int i = 0; i < N; i++)
				for (int j = 0; j < SIZE; j++)
					cout << p->mem_cache[i][j] << endl; //输出
			fclose(p->fp);

			Sleep(2000);
			ReleaseSemaphore(hSemaphore, 1, NULL); //释放信号量
												   //p->sem = 0;
		}
	}
	return 0;
}


DWORD WINAPI ThreadProc2(LPVOID lpParameter)
{
	Cache *q = (Cache *)lpParameter;

	for (int m = 0; m < q->num * N; m++)
	{
		WaitForSingleObject(hSemaphore, INFINITE);
		//if(q->sem == 0)
		//{
		cout << "run  2: " << endl;
		FILE *fw = fopen(Pathb, "wb");
		cout << "write:" << endl;
		for (int i = 0; i < N; i++)
		{
			fwrite(q->mem_cache[i], SIZE, 1, fw);
		}

		fclose(fw);
		//读完之后清空
		for (int i = 0; i < N; i++)
			memset(cache[i], '\0', SIZE);
		q->sem = 1;
		fclose(q->fp);
		ReleaseSemaphore(hSemaphore, 1, NULL); //释放信号量
	}
	return (0);
}



int main(int argc, char *argv[])
{
	int head = 0;
	int end = 1; //作为行
	for (int i = 0; i < N; i++)
		memset(cache[i], '/0', SIZE);
	//Cache(char a[N][SIZE], int Head, int End, int s, string s)
	//创建一个信号量，2是初值，3是最大值
	hSemaphore = CreateSemaphore(NULL, 2, 3, NULL);
	Cache *buffer = new Cache(cache, 1, Patha);

	HANDLE hThread1 = CreateThread(NULL, 0, ThreadProc1, (void *)buffer, 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, 0, ThreadProc2, (void *)buffer, 0, NULL);

	CloseHandle(hThread1);
	CloseHandle(hThread2);
	system("pause");
	return 0;
}
