//#include "stdafx.h"
#include "iostream"
#include "windows.h"
#include "string"
using namespace std;

DWORD WINAPI createThd1(LPVOID para)
{
    cout << "Thread1" << endl;
    Sleep(500);
    cout << "2000" << endl;
    int count = 0;
    while(1)
    {
        cout << count << endl;
        count ++;
        Sleep(1000);
    }
    // string *get = (string*)para;
    // string a = *get;
    // cout << a << endl;
}

DWORD WINAPI createThd2(LPVOID para)
{
    cout << "Thread2" << endl;
}


int main()
{
    // Sleep(1000);
    // HANDLE mainThread2 = CreateThread(NULL, 0, createThd2, NULL, 0, NULL);
    // Sleep(1000);
    // cout << "main " << endl;
    // string input;
    // cin >> input;
    string *pnt  = NULL;

    HANDLE mainThread1 = CreateThread(NULL, 0, createThd1, (void*)pnt, 0, NULL);
    Sleep(10000);
    CloseHandle(mainThread1);
    // CloseHandle(mainThread2);
    return 0;
}
