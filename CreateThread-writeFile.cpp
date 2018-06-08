// CreateThread-writeFile.cpp : �������̨Ӧ�ó������ڵ㡣
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
#define SIZE 256 //5kB�ϴ�Ļ�����
#define N 6
char cache[N][SIZE];
int read = 0; //��һ����������ͷָ��
			  //�����ź��� 1 ���Դ�a.txt�ļ��� 0 ������

//����һ���ź���
HANDLE hSemaphore;

class Cache
{
  public:
	char mem_cache[N][SIZE];
	int size;
	// int head;
	// int end;
	int sem; //�ź���
	FILE *fp;
	int num; //��ѭ���Ĵ���
	int fileSize;
	Cache(char a[N][SIZE], int s, char *str)
	{
		for (int i = 0; i < N; i++)
			for (int j = 0; j < SIZE; j++)
				mem_cache[i][j] = a[i][j];
		// head = Head;
		// end = End;
		sem = s;
		//�ļ�ָ����Ҫ���� ����
		fp = fopen(str, "rb");
		fseek(fp, 0, SEEK_END); //��λ�ļ���ͷ
		fileSize = ftell(fp);   //��ǰָ�뵽File��β�ĳ���
		cout << "�ļ���С��" << fileSize << endl;
		rewind(fp);
		num = (fileSize / (N * SIZE)) + 1;
		cout << "ѭ��" << num << "��" << endl;
	}
};

//���ļ����߳�
DWORD WINAPI ThreadProc1(LPVOID lpParameter)
{

	Cache *p = (Cache *)lpParameter;
	if (p->mem_cache[0][0] == '\0')
		p->sem = 1;
	for (int k = 0; k < p->num; k++)
	{
		WaitForSingleObject(hSemaphore, INFINITE);
		cout << "����" << p->num << "   - " << k << "ѭ��" << endl;
		//if (p->sem == 1)
		//{
		//cout << "p->fileSIZE:  " << p->fileSize << "NSIZE  " << N*SIZE << endl;
		if (p->fileSize > N * SIZE) //������ڶ�ȡһ������������
		{
			for (int j = 0; j < N; j++) //ѭ������
				fread(p->mem_cache[j], SIZE, 1, p->fp);

			cout << "�����һ�ζ����" << endl;
			for (int i = 0; i < N; i++)
				for (int j = 0; j < SIZE; j++)
					cout << p->mem_cache[i][j] << endl; //���

			fclose(p->fp);
			Sleep(2000);
			ReleaseSemaphore(hSemaphore, 1, NULL); //�ͷ��ź���
												   //p->sem = 0;
		}
		else
		{
			int leftSize = N * SIZE - p->fileSize;
			int N_1 = leftSize / SIZE;			 //��
			int real_leftSize = leftSize % SIZE; //���� �����ͷ
			for (int i = 0; i < N_1; i++)
				fread(p->mem_cache[i], SIZE, 1, p->fp);
			fread(p->mem_cache[N_1], real_leftSize, 1, p->fp);

			for (int i = 0; i < N; i++)
				for (int j = 0; j < SIZE; j++)
					cout << p->mem_cache[i][j] << endl; //���
			fclose(p->fp);

			Sleep(2000);
			ReleaseSemaphore(hSemaphore, 1, NULL); //�ͷ��ź���
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
		//����֮�����
		for (int i = 0; i < N; i++)
			memset(cache[i], '\0', SIZE);
		q->sem = 1;
		fclose(q->fp);
		ReleaseSemaphore(hSemaphore, 1, NULL); //�ͷ��ź���
	}
	return (0);
}



int main(int argc, char *argv[])
{
	int head = 0;
	int end = 1; //��Ϊ��
	for (int i = 0; i < N; i++)
		memset(cache[i], '/0', SIZE);
	//Cache(char a[N][SIZE], int Head, int End, int s, string s)
	//����һ���ź�����2�ǳ�ֵ��3�����ֵ
	hSemaphore = CreateSemaphore(NULL, 2, 3, NULL);
	Cache *buffer = new Cache(cache, 1, Patha);

	HANDLE hThread1 = CreateThread(NULL, 0, ThreadProc1, (void *)buffer, 0, NULL);
	HANDLE hThread2 = CreateThread(NULL, 0, ThreadProc2, (void *)buffer, 0, NULL);

	CloseHandle(hThread1);
	CloseHandle(hThread2);
	system("pause");
	return 0;
}
