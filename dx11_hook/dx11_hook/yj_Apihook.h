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

	int l_adder = 0;//ԭ������ַ
	int l_funadder = 0;//��������ַ
	byte l_funbyte[5];//ԭ�����ֽ�
	int err = 0;
	unsigned int getFunRealAddr(LPVOID fun);

};

