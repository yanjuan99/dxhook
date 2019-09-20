#pragma once
#include<Windows.h>


class Apihook
{
public:
	Apihook();

	Apihook(const char* lpModuleName, const char* lpProcName, LPVOID fun);

	~Apihook();
public:

	void Hook();

	void unHook();

	bool if_hook(const char* lpModuleName, const char* lpProcName);

	int get_Err();

	void setHook(int adders, LPVOID fun);

protected:

	int l_adder = 0;//原函数地址
	int l_funadder = 0;//代理函数地址
	byte l_funbyte[5];//原函数字节
	int err = 0;
	unsigned int getFunRealAddr(LPVOID fun);

};

