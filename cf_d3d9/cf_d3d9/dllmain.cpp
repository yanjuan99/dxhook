#pragma once
#include<Windows.h>
#include<iostream>
#include<d3d9.h>
#include"yj_Apihook.h"
#include<D3dx9core.h>



#pragma comment(lib,"d3d9.lib") 
#pragma comment(lib,"D3dx9.lib") 

HRESULT WINAPI NewD3DXCreateFontA(LPDIRECT3DDEVICE9       pDevice,
	INT                     Height,
	UINT                    Width,
	UINT                    Weight,
	UINT                    MipLevels,
	BOOL                    Italic,
	DWORD                   CharSet,
	DWORD                   OutputPrecision,
	DWORD                   Quality,
	DWORD                   PitchAndFamily,
	LPCSTR                  pFaceName,
	LPD3DXFONT*             ppFont);

厌倦_Apihook hook_D3DXCreateFontA("d3dx9_29.dll", "D3DXCreateFontA", NewD3DXCreateFontA);
厌倦_Apihook hook_DrawIndexedPrimitive;

LPDIRECT3DDEVICE9 s_pDxdevice;

bool HOOK判断 = false;

HRESULT WINAPI NewDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDxdevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	HRESULT  ret = NULL;
	IDirect3DVertexBuffer9* pStreamData = NULL;
	UINT OffsetInBytes, Stride;

	hook_DrawIndexedPrimitive.unHook();
	if (pDxdevice->GetStreamSource(0, &pStreamData, &OffsetInBytes, &Stride) == D3D_OK)
	{
		pStreamData->Release();
	}

		D3DVIEWPORT9 viewP;
		pDxdevice->GetViewport(&viewP);

		DWORD ScreenCenterX = viewP.Width / 2;
		DWORD ScreenCenterY = viewP.Height / 2;

		D3DRECT rec16 = { ScreenCenterX - 5, ScreenCenterY, ScreenCenterX + 5, ScreenCenterY + 1 };
		D3DRECT rec17 = { ScreenCenterX, ScreenCenterY - 5, ScreenCenterX + 1,ScreenCenterY + 5 };
		pDxdevice->Clear(1, &rec16, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 0), 0, 0);//D3DXCOLOR(0, 255, 0, 0), 0, 0);//red
		pDxdevice->Clear(1, &rec17, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 0, 0), 0, 0);// D3DXCOLOR(255, 255, 0, 0), 0, 0);

	if (Stride == 44 || Stride == 40)
	{
		DWORD dwOldZEnable = D3DZB_TRUE;
		pDxdevice->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable);

		pDxdevice->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE); //关闭z缓冲

		pDxdevice->SetRenderState(D3DRS_LIGHTING, D3DZB_FALSE);//光照[禁用]

															   //	pDxdevice->SetRenderState(D3DRS_ALPHABLENDENABLE, D3DZB_FALSE);//关闭阿尔法混合技术

		pDxdevice->SetRenderState(D3DRS_ZFUNC, D3DZB_TRUE);//更新深度缓冲区[永远不覆盖]

														   //pDxdevice->SetPixelShader(Front);

		ret = pDxdevice->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

		pDxdevice->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);
		pDxdevice->SetRenderState(D3DRS_FILLMODE, 3);//填充模式[平面]
		pDxdevice->SetRenderState(D3DRS_ZFUNC, 4);//更新深度缓冲区[小于等于时覆盖]
												  //	pDxdevice->SetPixelShader(Back);
		hook_DrawIndexedPrimitive.Hook();

		return ret;
	}

	ret = pDxdevice->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
	
	hook_DrawIndexedPrimitive.Hook();
	return ret;
}


HRESULT WINAPI NewD3DXCreateFontA(LPDIRECT3DDEVICE9       pDevice,
	INT                     Height,
	UINT                    Width,
	UINT                    Weight,
	UINT                    MipLevels,
	BOOL                    Italic,
	DWORD                   CharSet,
	DWORD                   OutputPrecision,
	DWORD                   Quality,
	DWORD                   PitchAndFamily,
	LPCSTR                  pFaceName,
	LPD3DXFONT*             ppFont)
{

	if (HOOK判断 == false)
	{
		s_pDxdevice = pDevice;
		int	DrawIndexedPrimitive_adder = *(int*)(*(int*)s_pDxdevice + 328);
		hook_DrawIndexedPrimitive.setHook(DrawIndexedPrimitive_adder, NewDrawIndexedPrimitive);
		hook_DrawIndexedPrimitive.Hook();
		HOOK判断 = true;
	}
	hook_D3DXCreateFontA.unHook();
	return D3DXCreateFontA(pDevice, Height, Width, Weight, MipLevels, Italic, CharSet, OutputPrecision, Quality, PitchAndFamily, pFaceName, ppFont);
}

void  mainz()
{
	hook_D3DXCreateFontA.Hook();
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:

		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)mainz, NULL, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

extern "C" void  __declspec(dllexport) add()
{
	return;
}