#pragma once
#include <Windows.h>
#include <string>
#include <d3d11.h>
#include <iostream>
#include <vector>
#include <memory>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_win32.h"

#include "structure.hpp"
#include "color.hpp"

namespace ImGuiEx
{
	inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}
	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, false);
	}
}

namespace polykur
{
	namespace cRender
	{
		extern IDXGISwapChain* m_Swapchain;
		extern ID3D11Device* m_Device;
		extern ID3D11DeviceContext* m_Context;
		extern ID3D11RenderTargetView* m_RTV;
		extern HWND GameWnd;
		extern WNDPROC m_OldWndProc;
		extern ImFont* m_pFont;
		extern HRESULT hr;

		extern Color espcolor;

		extern int Width;
		extern int Height;

		inline ImU32 GetU32(Color _color)
		{
			return ((_color[3] & 0xff) << 24) + ((_color[2] & 0xff) << 16) + ((_color[1] & 0xff) << 8)
				+ (_color[0] & 0xff);
		}

		// Init SwapChain, ImGui
		void Initialize(IDXGISwapChain* swapChain);

		// RenderTarget Stuff
		void CreateRenderTarget();
		void ClearRenderTarget();

		// Frame Stuff
		void BeginFrame();
		void EndFrame();

		// Scene Stuff
		void BeginScene();
		void EndScene();

		// Resize Stuff
		void PreResize();
		void PostResize();

		// ImGui Menu
		void CreateStyle();
		void Menu();
		
		// Draw Stuff
		void DrawString(float fontSize, const geo::vector2& vec, Color color, bool bCenter, bool stroke, const char* pText, ...);
		void DrawLine(const geo::vector2& vecStart, const geo::vector2& vecEnd, Color color, float thickness = 1.5f);
		void DrawCircle(const geo::vector2& vecCenter, float radius, Color color, int num_seg, float thickness);
		void DrawSkeleton(uint64_t Mesh, FCameraCacheEntry CameraCache, Color color);
	}
}