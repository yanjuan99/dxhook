#include "yj_Apihook.h"

void Apihook::setHook(int adders, LPVOID fun)
{
	l_adder = adders;
	l_funadder = getFunRealAddr(fun);
	for (int i = 0; i < 5; i++)
	{
		l_funbyte[i] = *(byte*)(adders + i);
	}
}

Apihook::Apihook() {  }

Apihook::Apihook(const char* lpModuleName, const char* lpProcName, LPVOID fun)
{
	HMODULE  module = GetModuleHandleA(lpModuleName);
	if (!module)
	{
		module = LoadLibraryA(lpModuleName);
	}
	int adder = (int)GetProcAddress(module, lpProcName);
	if (!adder)
	{
		err = 0;
	}
	l_adder = adder;
	l_funadder = getFunRealAddr(fun);

	for (int i = 0; i < 5; i++)
	{
		l_funbyte[i] = *(byte*)(adder + i);
	}

	err = 1;
}
Apihook::~Apihook()
{
	l_adder = 0;
	l_funadder = 0;
}

void Apihook::Hook()
{
	DWORD shuxin;
	VirtualProtect((LPVOID)l_adder, 5, 64, &shuxin);

	*(byte*)l_adder = 0xe9;
	*(int*)(l_adder + 1) = (int)l_funadder - l_adder - 5;

	VirtualProtect((LPVOID)l_adder, 5, shuxin, 0);
}

void Apihook::unHook()
{
	DWORD shuxin;
	VirtualProtect((LPVOID)l_adder, 5, 64, &shuxin);
	for (size_t i = 0; i < 5; i++)
	{
		*(byte*)(l_adder + i) = l_funbyte[i];
	}

	VirtualProtect((LPVOID)l_adder, 5, shuxin, 0);
}

bool Apihook::if_hook(const char* lpModuleName, const char* lpProcName)
{
	int text = (int)GetProcAddress(GetModuleHandleA(lpModuleName), lpProcName);

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

unsigned int Apihook::getFunRealAddr(LPVOID fun)
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

int Apihook::get_Err()
{
	return err;
}