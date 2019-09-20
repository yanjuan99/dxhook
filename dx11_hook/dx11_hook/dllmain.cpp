#pragma once
#include<Windows.h>
#include<d3d11.h>
#include <d3dcompiler.h>
#include<iostream>
#include "yj_Apihook.h"

#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)

using namespace std;
void  __declspec(dllexport) add()
{
	return;
}

ID3D11DepthStencilState *on;
ID3D11DepthStencilState *off;

ID3D11PixelShader* psCrimson = NULL;
ID3D11PixelShader* psYellow = NULL;

ID3D11RasterizerState * rwState;
ID3D11RasterizerState * rsState;
ID3D11Device *pDevice;
//extern ID3D11DeviceContext *pContext;
ID3D11ShaderResourceView* ShaderResourceView;

int 最大 = 0,最小=0;

Apihook dx11_hook;
Apihook dx11_hookoCreateQuery;
//ID3D11DeviceContext *  dx11driver;

void dPrintf(LPCSTR lpformat, ...)
{
	char szStr[1024];

	va_list argList;

	va_start(argList, lpformat);

	ZeroMemory(szStr, _countof(szStr));

	vsprintf_s(szStr, _countof(szStr), lpformat, argList);

	OutputDebugStringA(szStr); //输出到Buffer，

	va_end(argList);
}


int asm_d(int 地址)
{
	int err;
	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery((PVOID)地址, &mbi, sizeof(mbi)))
	{
		err = 1;
		if (mbi.State == MEM_COMMIT)
		{
			err = 2;
			if (IsBadReadPtr((PVOID)地址, 4) == false)
			{
				err = 3;
				return *(PINT)地址;
			}
		}
	}
	dPrintf("Reborn [asm_d err :%d]", err);
	return 0;
}



HRESULT GenerateShader(ID3D11Device* pD3DDevice, ID3D11PixelShader** pShader, float r, float g, float b)
{
	char szCast[] = "struct VS_OUT"
		"{"
		" float4 Position : SV_Position;"
		" float4 Color : COLOR0;"
		"};"

		"float4 main( VS_OUT input ) : SV_Target"
		"{"
		" float4 fake;"
		" fake.a = 1.0f;"
		" fake.r = %f;"
		" fake.g = %f;"
		" fake.b = %f;"
		" return fake;"
		"}";
	ID3D10Blob* pBlob;
	char szPixelShader[1000];

	sprintf_s(szPixelShader, szCast, r, g, b);

	ID3DBlob* d3dErrorMsgBlob;

	HRESULT hr = D3DCompile(szPixelShader, sizeof(szPixelShader), "shader", NULL, NULL, "main", "ps_4_0", NULL, NULL, &pBlob, &d3dErrorMsgBlob);

	if (FAILED(hr))
		return hr;

	hr = pD3DDevice->CreatePixelShader((DWORD*)pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, pShader);

	if (FAILED(hr))
		return hr;

	return S_OK;
}


void creatDepthStencilState(ID3D11DeviceContext* pSwapChain)
{
	static bool bOnce = false;

	if (!bOnce)
	{

		pSwapChain->GetDevice(&pDevice);
		//pDevice->GetImmediateContext(&pContext);
		D3D11_DEPTH_STENCIL_DESC  stencilDesc;

		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;//将深度数据与现有深度数据进行比较的功能。
		stencilDesc.StencilEnable = true;//启用模板测试。
		stencilDesc.StencilReadMask = 0xFF;//识别用于读取模板数据的深度模板缓冲区的一部分。
		stencilDesc.StencilWriteMask = 0xFF;//识别用于写入模板数据的深度模板缓冲区的一部分。


											//确定如何使用表面法线朝向摄像机的像素进行深度测试和模板测试的结果
		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


		//确定如何使用深度测试和模板测试的结果来测量表面法线背离摄像机的像素
		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		stencilDesc.DepthEnable = true;//启用深度测试。
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;//识别深度数据可以修改的深度模板缓冲区的一部分

		pDevice->CreateDepthStencilState(&stencilDesc, &on);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		stencilDesc.DepthEnable = false;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		pDevice->CreateDepthStencilState(&stencilDesc, &off);



		if (!psCrimson)
			GenerateShader(pDevice, &psCrimson, 0, 0, 0);

		//if (!psYellow)
		//	GenerateShader(pDevice, &psYellow, 0, 0, 0);

		bOnce = true;
	}

}

void WINAPI D11DrawIndexed(ID3D11DeviceContext* pContext, UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	UINT Stride;
	ID3D11Buffer *veBuffer;
	UINT veBufferOffset = 0;

	ID3D11PixelShader* shader;
	ID3D11Buffer * index;
	D3D11_BUFFER_DESC desc;
	dx11_hook.unHook();
	creatDepthStencilState(pContext);

	pContext->IAGetVertexBuffers(0, 1, &veBuffer, &Stride, &veBufferOffset);
	pContext->IAGetIndexBuffer(&index, 0, 0);
	index->GetDesc(&desc);


	dPrintf("Reborn [desc.ByteWidth :%d]", desc.ByteWidth);

	if (Stride == 24
		&& desc.ByteWidth != 3684
		&& desc.ByteWidth != 3960
		&& desc.ByteWidth != 4932
		&& desc.ByteWidth != 4986
		&& desc.ByteWidth != 6114
		&& desc.ByteWidth != 4848
		&& desc.ByteWidth != 1200
		&& desc.ByteWidth != 2688
		&& desc.ByteWidth != 1326
		&& desc.ByteWidth != 2964
		&& desc.ByteWidth != 3270
		&& desc.ByteWidth != 3288
		&& desc.ByteWidth != 最小
		&& desc.ByteWidth != 最大
		&& desc.ByteWidth != 13692
		&& desc.ByteWidth != 2376
		&& desc.ByteWidth != 4392
		)
	{


			pContext->OMSetDepthStencilState(off, 1);


			pContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);


			pContext->OMSetDepthStencilState(on, 1);


			dx11_hook.Hook();
			return;
	
	}

	
	//if (Stride == 24)
	//{

	//		//	&& desc.ByteWidth != setStride)
	//	

	//		//if (
	//		//	desc.ByteWidth >= 0x5B04
	//		//	&& desc.ByteWidth <= 0x5B68
	//		//	|| desc.ByteWidth >= 0x80E8 && desc.ByteWidth <= 0x84D0)
	//		//{

	//		pContext->OMSetDepthStencilState(on, 1);

	//		//	pContext->PSGetShader(&shader, 0, 0);

	//	//		pContext->PSSetShader(psCrimson, 0, 0);

	//		pContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);


	//		pContext->OMSetDepthStencilState(off, 1);


	//		//		pContext->PSSetShader(shader, 0, 0);


	//			//	pContext->PSSetShaderResources(0, 1, &ShaderResourceView);

	//		dx11_hook.Hook();
	//		return;
	//		//}
	//	
	//}

	pContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
	dx11_hook.Hook();
	return;
}

void __stdcall hkD3D11CreateQuery(ID3D11Device* pDevice, const D3D11_QUERY_DESC *pQueryDesc, ID3D11Query **ppQuery)
{

	dx11_hookoCreateQuery.unHook();

	if (pQueryDesc->Query == D3D11_QUERY_OCCLUSION)
	{
		D3D11_QUERY_DESC oqueryDesc = CD3D11_QUERY_DESC();
		(&oqueryDesc)->MiscFlags = pQueryDesc->MiscFlags;
		(&oqueryDesc)->Query = D3D11_QUERY_TIMESTAMP;
		pDevice->CreateQuery(&oqueryDesc, ppQuery);
		dx11_hookoCreateQuery.Hook();
		return;

	}
	pDevice->CreateQuery(pQueryDesc, ppQuery);
	dx11_hookoCreateQuery.Hook();
	return;
}


void  main()
{
	dPrintf("Reborn 最大 %X __ 最小%X", &最大 ,&最小);

	int hook地址 = (int)GetModuleHandleA("d3d11.dll") + 0x171B40;
	dx11_hook.setHook(hook地址, D11DrawIndexed);
	dx11_hook.Hook();

	int oCreateQuery = (int)GetModuleHandleA("d3d11.dll") + 0x2BD80;;
	dx11_hookoCreateQuery.setHook(oCreateQuery, hkD3D11CreateQuery);
	dx11_hookoCreateQuery.Hook();
}


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&main, NULL, 0, 0);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}