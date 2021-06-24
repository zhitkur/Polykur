#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <d3d11.h>
#include "utils.hpp"
#include "render.hpp"
#include "minhook/MinHook.h"

#pragma comment (lib, "MinHook.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3d11.lib")

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_internal.h"

#include "config.hpp"

/* DX11�� ����ϱ� ���� �ʿ��� ������Ʈ
1. Device - 3D �׷��� ī��� ����Ǵ� �⺻ ����̽� ��ü 
(DX9�� ����̽��� ���� ����������, DX11���ʹ� Multi-Thread�� ����ȭ�ϱ� ���� ���� ����̽��� ������� �ʰ�,  Context�� �̿��Ͽ� Device�� ����� ����)
2. Immediate Context - DX11�� ���� �߰��� ������Ʈ
(���� ���ߵ��� �� ��ü�� ���� Device�� ����� ������)
3. SwapChain - ȭ�鿡 �������� Backbuffer�� �����ϰ�, ������ ȭ�鿡 �������ϴ� ������ ����ϴ� ������Ʈ
(�� ������Ʈ�� �̿��Ͽ� Backbuffer�� Render taget�� �� Target window�� �������ϴ� ����)
*/

/*	DX11 �⺻���� Setup ����
1. ID3D11Device, IDXGISwapChain�� �����Ѵ�.	- IDXGISwapChain* GetSwapChain()
2. SwapChain���� Backbuffer�� �����´�.		- void CreateRenderTarget() : m_Swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
3. Backbuffer���� ���� �������� "RenderTargetView"�� �����Ͽ� Binding ��Ų��. - void CreateRenderTarget() : m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_RTV);
4. Viewport ���� (���⼱ ��ŷ�� ���� �׸��⸦ �ϱ� ������ ����)
5. �� �����Ӹ��� ������ ������ "RenderTargetView"�� ���� ȭ�鿡 �������Ѵ�.
6. SwapChain�� �̿��Ͽ� ����̽����� ȭ���� �׸���.

�� �������� ���� �߿��� ������ "RenderTargetView"�� �����ϴ� �� 
��? -> "RenderTargetView"�� �ᱹ �츮�� 3D ���� ȭ��(Target window)�� �������� Ÿ���� �ȴ�.
*/


/* IDXGISwapChain virtuals (SC_ prefix for swapchain)
#define SC_QUERYINTERFACE            0
#define SC_ADDREF                    1
#define SC_RELEASE                   2
#define SC_SETPRIVATEDATA            3
#define SC_SETPRIVATEDATAINTERFACE   4
#define SC_GETPRIVATEDATA            5
#define SC_GETPARENT                 6
#define SC_GETDEVICE                 7
#define SC_PRESENT                   8
#define SC_GETBUFFER                 9
#define SC_SETFULLSCREENSTATE        10
#define SC_GETFULLSCREENSTATE        11
#define SC_GETDESC                   12
#define SC_RESIZEBUFFERS             13
#define SC_RESIZETARGET              14
#define SC_GETCONTAININGOUTPUT       15
#define SC_GETFRAMESTATISTICS        16
#define SC_GETLASTPRESENTCOUNT       17
*/

using namespace polykur;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (g_MenuOpened && ImGui_ImplWin32_WndProcHandler(cRender::GameWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(cRender::m_OldWndProc, hWnd, uMsg, wParam, lParam);
}

static IDXGISwapChain* GetSwapChain()
{
	HMODULE hDXGIDLL = 0;
	do
	{
		hDXGIDLL = GetModuleHandle(E("dxgi.dll"));
		Sleep(1);
	} while (!hDXGIDLL);

	/* DXGI_SWAP_CHAIN_DESC
	- �Ʒ��� â����� ����۸� 1�� ������ SwapChain�� �����ϴ� ����
	*/
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));										// Init struct - ����ü �ʱ�ȭ
	swapDesc.BufferCount = 1;														// Backbuffer count - ����� ����
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						// Backbuffer format (RGBA 8bit) - ����� ����
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							// How to using backbuffer - ����۸� � �뵵�� ����� ����
	swapDesc.OutputWindow = FindWindow((LPCSTR)E("UnrealWindow"), (LPCSTR)nullptr);	// Window to output buffer - ���۸� ����� ������
	swapDesc.SampleDesc.Count = 1;													// Multi sampling count - ��Ƽ ���ø� ��
	swapDesc.Windowed = TRUE;														// Windowed Mode (TRUE, FALSE) - â��� Ȥ�� ��üȭ��

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	IDXGISwapChain* pSwapChain = 0;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,	// Driver type
		NULL,
		NULL,
		&featureLevel,				// Feature level (11_0, 10_1, 10_0, 9_3, 9_2, 9_1) �ش� ������ �´� ����̽����� ȣȯ�� �����ϵ��� ��
		1,							// Feature level count
		D3D11_SDK_VERSION,			// SDK version 
		&swapDesc,					
		&pSwapChain,
		&pDevice,
		NULL,
		&pContext)))
	{
		pContext->Release();
		pDevice->Release();
		return pSwapChain;
	}
	else
		return NULL;
}

extern void* o_present;
HRESULT hk_present(IDXGISwapChain* dis, UINT syncInterval, UINT flags);
extern void* o_resizebuffers;
HRESULT hk_resizebuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);

inline DWORD_PTR* pSwapChainVtable;
inline uint64_t dwPresent;
inline uint64_t dwResizeBuffers;

static void HookD3D11()
{
	IDXGISwapChain* m_pSwapChain = GetSwapChain();

	if (m_pSwapChain)
	{
		G::ThreadStuff(true);
		pSwapChainVtable = (DWORD_PTR*)m_pSwapChain;
		pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];
		dwPresent = (uint64_t)(DWORD_PTR*)pSwapChainVtable[8];			// vTable[8]  - Present
		dwResizeBuffers = (uint64_t)(DWORD_PTR*)pSwapChainVtable[13];	// vTable[13] - ResizeBuffers

		DWORD OldProtect{}, OldProtect2{};
		VirtualProtect((void*)dwPresent, 2, PAGE_EXECUTE_READWRITE, &OldProtect);
		VirtualProtect((void*)dwResizeBuffers, 2, PAGE_EXECUTE_READWRITE, &OldProtect2);

		MH_Initialize();
		MH_CreateHook((void*)dwPresent, (void*)hk_present, reinterpret_cast<void**>(&o_present));	// Hook present
		MH_EnableHook((void*)dwPresent);
		MH_CreateHook((void*)dwResizeBuffers, (void*)hk_resizebuffers, reinterpret_cast<void**>(&o_resizebuffers));	// Hook resizebuffers
		MH_EnableHook((void*)dwResizeBuffers);

		VirtualProtect((void*)dwPresent, 2, OldProtect, &OldProtect);
		VirtualProtect((void*)dwResizeBuffers, 2, OldProtect2, &OldProtect2);
		G::ThreadStuff(false);
	}
}