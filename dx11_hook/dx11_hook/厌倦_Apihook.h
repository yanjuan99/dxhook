#pragma once
#include<Windows.h>


class ���_Apihook
{
public:
	void Hook();

	void unHook();

	bool if_hook(char lpModuleName[], char lpProcName[]);

	int get_Err();

	void setHook(int adders, LPVOID fun);

	���_Apihook();

	���_Apihook(char lpModuleName[], char lpProcName[], LPVOID fun);

	~���_Apihook();

protected:

	int l_adder = 0;//ԭ������ַ
	int l_funadder = 0;//�Զ���fun��ַ
	byte l_funbyte[5];//ԭ�����ֽ�
	int err = 0;
	unsigned int getFunRealAddr(LPVOID fun);

};

