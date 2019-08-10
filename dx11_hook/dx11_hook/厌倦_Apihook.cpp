#include "Ñá¾ë_Apihook.h"

void Ñá¾ë_Apihook::setHook(int adders, LPVOID fun)
{
	if (adders==0)
	{
		MessageBoxA(0, "HOOKµØÖ·²»ÄÜÎª0","txt", 0);
		return;
	}
	Ñá¾ë_Apihook::l_adder = adders;
	Ñá¾ë_Apihook::l_funadder = Ñá¾ë_Apihook::getFunRealAddr(fun);
	for (int i = 0; i < 5; i++)
	{
		Ñá¾ë_Apihook::l_funbyte[i] = *(byte*)(adders + i);
	}
}

Ñá¾ë_Apihook::Ñá¾ë_Apihook() {  }

Ñá¾ë_Apihook::Ñá¾ë_Apihook(char lpModuleName[], char lpProcName[], LPVOID fun)
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
		Ñá¾ë_Apihook::err = 0;
	}
	Ñá¾ë_Apihook::l_adder = adder;
	Ñá¾ë_Apihook::l_funadder = Ñá¾ë_Apihook::getFunRealAddr(fun);

	for (int i = 0; i < 5; i++)
	{
		Ñá¾ë_Apihook::l_funbyte[i] = *(byte*)(adder + i);
	}

	Ñá¾ë_Apihook::err = 1;
}
Ñá¾ë_Apihook::~Ñá¾ë_Apihook()
{
	Ñá¾ë_Apihook::l_adder = 0;
	Ñá¾ë_Apihook::l_funadder = 0;
}

void Ñá¾ë_Apihook::Hook()
{
	DWORD shuxin;
	VirtualProtect((LPVOID)Ñá¾ë_Apihook::l_adder, 5, 64, &shuxin);

	*(byte*)Ñá¾ë_Apihook::l_adder = 0xe9;
	*(int*)(Ñá¾ë_Apihook::l_adder + 1) = (int)Ñá¾ë_Apihook::l_funadder - Ñá¾ë_Apihook::l_adder - 5;

	VirtualProtect((LPVOID)Ñá¾ë_Apihook::l_adder, 5, shuxin, 0);
}

void Ñá¾ë_Apihook::unHook()
{
	DWORD shuxin;
	VirtualProtect((LPVOID)Ñá¾ë_Apihook::l_adder, 5, 64, &shuxin);
	for (size_t i = 0; i < 5; i++)
	{
		*(byte*)(Ñá¾ë_Apihook::l_adder + i) = Ñá¾ë_Apihook::l_funbyte[i];
	}

	VirtualProtect((LPVOID)Ñá¾ë_Apihook::l_adder, 5, shuxin, 0);
}

bool Ñá¾ë_Apihook::if_hook(char lpModuleName[], char lpProcName[])
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

unsigned int Ñá¾ë_Apihook::getFunRealAddr(LPVOID fun)
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

int Ñá¾ë_Apihook::get_Err()
{
	return Ñá¾ë_Apihook::err;
}