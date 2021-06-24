/* made by zhitkur 21.06.25
* I'm a very noob at game hacking, please understand even if the code is shitty.
* Discord: Çãµ¹µ¹#0001 (If you don't have Korean on your keyboard, C&P)
* Based Code: https://github.com/he4rtbleed/Prestige
*/

#include <Windows.h>
#include <thread>
#include <iostream>
#include <mutex>
#include <iomanip>

#include "hooks.hpp"
#include "render.hpp"
#include "config.hpp"
#include "executor.hpp"
#include "core.hpp"

#pragma warning (disable : 4244)

using namespace polykur;

using LSR = LPTHREAD_START_ROUTINE;

std::once_flag once;

void* o_present;


HRESULT hk_present(IDXGISwapChain* dis, UINT syncInterval, UINT flags)
{
	std::call_once(once, [dis]() {
		Core::Init();
		cRender::Initialize(dis);
		});

	if (g_Running)
	{
		cRender::BeginFrame();

		// Render Menu
		if (g_MenuOpened)
			cRender::Menu();

		cRender::BeginScene();

		// Function Executor (ESP, Watermark)
		function::Executor();

		cRender::EndScene();
		cRender::EndFrame();

	}

	return reinterpret_cast<decltype(&hk_present)>(o_present)(dis, syncInterval, flags);
}

void* o_resizebuffers;
HRESULT hk_resizebuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
{
	if (g_Running)
	{
		cRender::PreResize();
		auto hr = reinterpret_cast<decltype(&hk_resizebuffers)>(o_resizebuffers)(dis, bufferCount, width, height, newFormat, swapChainFlags);
		cRender::PostResize();

		return hr;
	}

	return reinterpret_cast<decltype(&hk_resizebuffers)>(o_resizebuffers)(dis, bufferCount, width, height, newFormat, swapChainFlags);
}

void OnDllAttach()
{
#ifdef _DEBUG
	AllocConsole();
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	SetConsoleTitle(G::random_string(20).c_str());
#endif // _DEBUG

	using namespace std::chrono;

	HookD3D11();

	while (g_Running) {
		if (IsKeyPressed(VK_END))
		{
			g_Running = false;
		}

		if (GetAsyncKeyState(VK_F1) & 1)
			g_MenuOpened = !g_MenuOpened;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	FreeLibraryAndExitThread(G::g_Module, 0);
}

void OnDllDetach()
{
#ifdef _DEBUG
	fclose((FILE*)stdin);
	fclose((FILE*)stdout);

	HWND hWnd = GetConsoleWindow();
	FreeConsole();
	PostMessageW(hWnd, WM_CLOSE, 0, 0);
#endif // _DEBUG

	SetWindowLongPtrA(cRender::GameWnd, GWLP_WNDPROC, (LONG_PTR)cRender::m_OldWndProc);

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	cRender::ClearRenderTarget();

	MH_Uninitialize();
	MH_DisableHook((void*)dwPresent);
	MH_DisableHook((void*)dwResizeBuffers);
}


BOOL DllMain(HINSTANCE hInstance, unsigned long Reason, void* reserved)
{
	if (Reason == DLL_PROCESS_ATTACH)
	{
		G::g_Module = hInstance;
		CreateThread(nullptr, NULL, reinterpret_cast<LSR>(OnDllAttach), G::g_Module, NULL, nullptr);
	}

	else if (Reason == DLL_PROCESS_DETACH)
	{
		if (!reserved)
		{
			using namespace std::literals::chrono_literals;
			std::this_thread::sleep_for(0.2s);

			// Unhook, detach console
			OnDllDetach();
			Beep(500, 200);
		}
	}

	return TRUE;
}