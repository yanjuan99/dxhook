#pragma once
#include<Windows.h>


class 厌倦_Apihook
{
public:
	void Hook();

	void unHook();

	bool if_hook(char lpModuleName[], char lpProcName[]);

	int get_Err();

	void setHook(int adders, LPVOID fun);

	厌倦_Apihook();

	厌倦_Apihook(char lpModuleName[], char lpProcName[], LPVOID fun);

	~厌倦_Apihook();

protected:

	int l_adder = 0;//原函数地址
	int l_funadder = 0;//自定义fun地址
	byte l_funbyte[5];//原函数字节
	int err = 0;
	unsigned int getFunRealAddr(LPVOID fun);

};

