#include "���_Apihook.h"

void ���_Apihook::setHook(int adders, LPVOID fun)
{
	if (adders==0)
	{
		MessageBoxA(0, "HOOK��ַ����Ϊ0","txt", 0);
		return;
	}
	���_Apihook::l_adder = adders;
	���_Apihook::l_funadder = ���_Apihook::getFunRealAddr(fun);
	for (int i = 0; i < 5; i++)
	{
		���_Apihook::l_funbyte[i] = *(byte*)(adders + i);
	}
}

���_Apihook::���_Apihook() {  }

���_Apihook::���_Apihook(char lpModuleName[], char lpProcName[], LPVOID fun)
{
	char* z_lpModuleName = lpModuleName;
	char* z_lpProcName = lpProcName;
	HMODULE  module = GetModuleHandleA(z_lpModuleName);
	if (module == 0)
	{
		module = LoadLibraryA(z_lpModuleName);
	}
	int adder = (int)GetProcAddress(module, z_lpProcName);
	if (adder == 0)
	{
		���_Apihook::err = 0;
	}
	���_Apihook::l_adder = adder;
	���_Apihook::l_funadder = ���_Apihook::getFunRealAddr(fun);

	for (int i = 0; i < 5; i++)
	{
		���_Apihook::l_funbyte[i] = *(byte*)(adder + i);
	}

	���_Apihook::err = 1;
}
���_Apihook::~���_Apihook()
{
	���_Apihook::l_adder = 0;
	���_Apihook::l_funadder = 0;
}

void ���_Apihook::Hook()
{
	DWORD shuxin;
	VirtualProtect((LPVOID)���_Apihook::l_adder, 5, 64, &shuxin);

	*(byte*)���_Apihook::l_adder = 0xe9;
	*(int*)(���_Apihook::l_adder + 1) = (int)���_Apihook::l_funadder - ���_Apihook::l_adder - 5;

	VirtualProtect((LPVOID)���_Apihook::l_adder, 5, shuxin, 0);
}

void ���_Apihook::unHook()
{
	DWORD shuxin;
	VirtualProtect((LPVOID)���_Apihook::l_adder, 5, 64, &shuxin);
	for (size_t i = 0; i < 5; i++)
	{
		*(byte*)(���_Apihook::l_adder + i) = ���_Apihook::l_funbyte[i];
	}

	VirtualProtect((LPVOID)���_Apihook::l_adder, 5, shuxin, 0);
}

bool ���_Apihook::if_hook(char lpModuleName[], char lpProcName[])
{
	char* z_lpModuleName = lpModuleName;
	char* z_lpProcName = lpProcName;

	int text = (int)GetProcAddress(GetModuleHandleA(z_lpModuleName), z_lpProcName);

	for (int i = 0; i < 8; i++)
	{
		if (*(byte*)(text + i) == 0xe9)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

unsigned int ���_Apihook::getFunRealAddr(LPVOID fun)
{
	unsigned int realaddr = (unsigned int)fun;

	unsigned char* funaddr = (unsigned char*)fun;
	if (funaddr[0] == 0xE9)
	{
		int disp = *(int*)(funaddr + 1);
		realaddr += 5 + disp;
	}
	return realaddr;
}

int ���_Apihook::get_Err()
{
	return ���_Apihook::err;
}