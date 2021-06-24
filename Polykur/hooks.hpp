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

/* DX11를 사용하기 위해 필요한 오브젝트
1. Device - 3D 그래픽 카드와 연결되는 기본 디바이스 객체 
(DX9의 디바이스와 같은 역할이지만, DX11부터는 Multi-Thread를 최적화하기 위해 직접 디바이스를 사용하지 않고,  Context를 이용하여 Device에 명령을 내림)
2. Immediate Context - DX11에 새로 추가된 오브젝트
(위에 말했듯이 이 객체를 통해 Device에 명령을 내린다)
3. SwapChain - 화면에 렌더링할 Backbuffer를 관리하고, 실제로 화면에 렌더링하는 역할을 담당하는 오브젝트
(위 오브젝트를 이용하여 Backbuffer의 Render taget을 얻어내 Target window에 렌더링하는 역할)
*/

/*	DX11 기본적인 Setup 순서
1. ID3D11Device, IDXGISwapChain을 생성한다.	- IDXGISwapChain* GetSwapChain()
2. SwapChain에서 Backbuffer를 가져온다.		- void CreateRenderTarget() : m_Swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
3. Backbuffer에서 실제 렌더링할 "RenderTargetView"를 생성하여 Binding 시킨다. - void CreateRenderTarget() : m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_RTV);
4. Viewport 생성 (여기선 후킹을 통한 그리기를 하기 때문에 생략)
5. 매 프레임마다 위에서 생성한 "RenderTargetView"를 게임 화면에 렌더링한다.
6. SwapChain을 이용하여 디바이스에서 화면을 그린다.

위 순서에서 가장 중요한 목적은 "RenderTargetView"를 생성하는 것 
왜? -> "RenderTargetView"가 결국 우리가 3D 게임 화면(Target window)을 렌더링할 타겟이 된다.
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
	- 아래는 창모드의 백버퍼를 1개 가지는 SwapChain을 설정하는 예시
	*/
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(swapDesc));										// Init struct - 구조체 초기화
	swapDesc.BufferCount = 1;														// Backbuffer count - 백버퍼 개수
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;						// Backbuffer format (RGBA 8bit) - 백버퍼 포맷
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;							// How to using backbuffer - 백버퍼를 어떤 용도로 사용할 건지
	swapDesc.OutputWindow = FindWindow((LPCSTR)E("UnrealWindow"), (LPCSTR)nullptr);	// Window to output buffer - 버퍼를 출력할 윈도우
	swapDesc.SampleDesc.Count = 1;													// Multi sampling count - 멀티 샘플링 수
	swapDesc.Windowed = TRUE;														// Windowed Mode (TRUE, FALSE) - 창모드 혹은 전체화면

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	IDXGISwapChain* pSwapChain = 0;
	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;
	if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,	// Driver type
		NULL,
		NULL,
		&featureLevel,				// Feature level (11_0, 10_1, 10_0, 9_3, 9_2, 9_1) 해당 버전에 맞는 디바이스여도 호환이 가능하도록 함
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