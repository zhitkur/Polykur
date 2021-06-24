#include "render.hpp"
#include "font.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "hooks.hpp"
#include "core.hpp"

#include <D3DX11tex.h>

#pragma warning(disable : 4244)

// IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace polykur
{
	IDXGISwapChain* cRender::m_Swapchain;
	ID3D11Device* cRender::m_Device;
	ID3D11DeviceContext* cRender::m_Context;
	ID3D11RenderTargetView* cRender::m_RTV;
	HWND cRender::GameWnd;
	WNDPROC cRender::m_OldWndProc;
	ImFont* cRender::m_pFont;
	HRESULT cRender::hr;

	Color cRender::espcolor(0xFF00FF7F);

	int cRender::Width;
	int cRender::Height;

	void cRender::Initialize(IDXGISwapChain* swapChain)
	{
		m_Swapchain = swapChain;
		if (FAILED(m_Swapchain->GetDevice(__uuidof(ID3D11Device), (void**)(&m_Device))))
			return;

		m_Device->GetImmediateContext(&m_Context);

		GameWnd = FindWindow(E("UnrealWindow"), NULL);

		RECT rct;
		GetClientRect(GameWnd, &rct);
		Width = rct.right - rct.left;
		Height = rct.bottom - rct.top;

		CreateRenderTarget();

		cRender::m_OldWndProc = (WNDPROC)SetWindowLongPtrA(cRender::GameWnd, GWLP_WNDPROC, (LONG_PTR)hkWndProc);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(cRender::GameWnd);
		ImGui_ImplDX11_Init(m_Device, m_Context);

		m_pFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(Cousine_compressed_data, Cousine_compressed_size, 14.f);

		cRender::CreateStyle();
	}

	void cRender::CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		m_Swapchain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));	// Backbuffer¸¦ ¹Þ¾Æ¿Ã º¯¼ö - pBackBuffer
		m_Device->CreateRenderTargetView(pBackBuffer, NULL, &m_RTV);
		pBackBuffer->Release();
	}

	void cRender::ClearRenderTarget()
	{
		if (m_RTV)
		{
			m_RTV->Release();
			m_RTV = NULL;
		}
	}

	void cRender::BeginFrame()
	{
		if (g_MenuOpened)
		{
			// Enable Mouse
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
		else
		{
			// Disable Mouse
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void cRender::EndFrame()
	{
		ImGui::Render();
		m_Context->OMSetRenderTargets(1, &m_RTV, NULL);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}


	void cRender::BeginScene()
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
		ImGui::Begin("##Backbuffer", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);

		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
	}

	void cRender::EndScene()
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DrawList->PushClipRectFullScreen();

		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::PopStyleVar(2);
	}

	void cRender::PreResize()
	{
		RECT rct;
		GetClientRect(GameWnd, &rct);
		Width = rct.right - rct.left;
		Height = rct.bottom - rct.top;
		ImGui_ImplDX11_InvalidateDeviceObjects();
		ClearRenderTarget();
	}

	void cRender::PostResize()
	{
		CreateRenderTarget();
		ImGui_ImplDX11_CreateDeviceObjects();
	}

	void cRender::CreateStyle()
	{
		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowPadding = ImVec2(15, 5);
		style->WindowRounding = 0.5f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 0.5f;
		style->ItemSpacing = ImVec2(12, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 9.0f;
		style->GrabMinSize = 5.0f;
		style->GrabRounding = 3.0f;

		ImVec4* Colors = style->Colors;

		Colors[ImGuiCol_Text] = ImColor(135, 255, 212, 255);
		Colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 220);
		Colors[ImGuiCol_TitleBgActive] = ImColor(18, 7, 37, 255);
		Colors[ImGuiCol_Border] = ImColor(0, 0, 0, 255);
		Colors[ImGuiCol_Tab] = ImColor(45, 20, 63, 255);
		Colors[ImGuiCol_TabHovered] = ImColor(113, 10, 102, 189);
		Colors[ImGuiCol_TabActive] = ImColor(169, 49, 132, 235);
		Colors[ImGuiCol_FrameBg] = ImColor(28, 96, 100, 62);
		Colors[ImGuiCol_FrameBgHovered] = ImColor(52, 117, 94, 102);
		Colors[ImGuiCol_FrameBgActive] = ImColor(42, 172, 53, 171);
		Colors[ImGuiCol_ScrollbarGrab] = ImColor(48, 12, 36, 255);
		Colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(113, 4, 76, 255);
		Colors[ImGuiCol_Button] = ImColor(89, 91, 137, 102);
		Colors[ImGuiCol_ButtonHovered] = ImColor(56, 78, 103, 255);
		Colors[ImGuiCol_CheckMark] = ImColor(255, 0, 118, 152);
		Colors[ImGuiCol_SliderGrab] = ImColor(255, 0, 118, 152);
		Colors[ImGuiCol_ResizeGrip] = ImColor(0, 0, 0, 42);
		//Colors[ImGuiCol_Separator] = ImColor(1, 0, 77, 128);
		Colors[ImGuiCol_Separator] = ImColor(9, 217, 183, 128);
	}

	void cRender::Menu()
	{
		ImGui::SetNextWindowPos(ImVec2(1450, 50), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Once);

		std::stringstream ExpireDate;
		ExpireDate << E("zhitkur - ") << E(__DATE__);

		ImGui::Begin(ExpireDate.str().c_str(), NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);
		ImGui::SetWindowSize(ImVec2(330.5f, 400.f));

		if (ImGui::BeginChild(E("##Stng"), ImVec2(0, 360), true, ImGuiWindowFlags_NoScrollbar))
		{
			if (ImGui::Button(E("VISUALS"), ImVec2(85, 25)))
			{
				g_TabCount = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button(E("AIMBOT"), ImVec2(85, 25)))
			{
				g_TabCount = 1;
			}
			ImGui::SameLine();

			if (ImGui::Button(E("MISC"), ImVec2(85, 25)))
			{
				g_TabCount = 2;
			}
			ImGui::Separator();

			if (g_TabCount == 0)
			{
				auto& style = ImGui::GetStyle();
				float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2 + 100;
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnOffset(1, group_w / 3.0f);
				ImGui::SetColumnOffset(2, 2 * group_w / 1.2f);

				ImGui::Checkbox(E("ESP"), &Vars.visuals.ESP);
				ImGui::Checkbox(E("Snapline"), &Vars.visuals.Snapline);
				ImGui::Checkbox(E("Skeleton"), &Vars.visuals.Skeleton);
				ImGui::NextColumn();
				ImGui::Checkbox(E("Team ESP"), &Vars.visuals.TeamESP);
				ImGui::Checkbox(E("Vis Check(X)"), &Vars.visuals.vis_check);


				ImGui::Columns(1, nullptr, false);
			}

			if (g_TabCount == 1)
			{
				auto& style = ImGui::GetStyle();
				float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2 + 100;
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnOffset(1, group_w / 3.0f);
				ImGui::SetColumnOffset(2, 2 * group_w / 1.2f);

				ImGui::Checkbox(E("Aimbot(X)"), &Vars.aimbot.Aimbot);
				ImGui::Checkbox(E("FOV"), &Vars.aimbot.fov);
				ImGui::NextColumn();
				ImGui::Checkbox(E("Slient Aim(X)"), &Vars.aimbot.SilentAim);
				ImGui::Checkbox(E("No Recoil"), &Vars.aimbot.no_recoil);
				ImGui::Columns(1, nullptr, false);
				ImGui::Separator();
				ImGui::SliderFloat(E("FOV Size"), &Vars.aimbot.fov_size, 30.f, 300.f, "%.f");

				ImGui::Columns(1, nullptr, false);
			}

			if (g_TabCount == 2)
			{
				auto& style = ImGui::GetStyle();
				float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2 + 100;
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnOffset(1, group_w / 3.0f);
				ImGui::SetColumnOffset(2, 2 * group_w / 1.5f);
				//ImGui::SetColumnOffset(2, 2 * group_w / 2.f);

				ImGui::Checkbox(E("Show FPS"), &Vars.misc.show_fps);

				ImGui::NextColumn();
				ImGui::Checkbox(E("Max Stamina(X)"), &Vars.misc.max_stamina);
				//ImGui::Checkbox("Rapid fire(X)", &Vars.misc.rapid_fire);

				ImGui::Columns(1, nullptr, false);
				ImGui::Separator();

				style = ImGui::GetStyle();
				group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2 + 100;
				ImGui::Columns(2, nullptr, false);
				ImGui::SetColumnOffset(1, group_w / 3.0f);
				ImGui::SetColumnOffset(2, 2 * group_w / 1.5f);

				ImGui::Columns(1, nullptr, false);

				if (ImGui::Button(E("SAVE CFG"), ImVec2(130, 25)))
				{
					Beep(400, 100);
					Config->Save();
				}
				ImGui::SameLine();
				if (ImGui::Button(E("LOAD CFG"), ImVec2(130, 25)))
				{
					Beep(400, 100);
					Config->Load();
				}
			}
			ImGui::EndChild();

			ImGui::End();
		}
	}

	void cRender::DrawString(float fontSize, const geo::vector2& vec, Color color, bool bCenter, bool stroke, const char* pText, ...)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		va_list va_alist;
		char buf[1024] = { 0 };
		va_start(va_alist, pText);
		_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
		va_end(va_alist);
		std::string text = Hexto::WStringToUTF8(Hexto::MBytesToWString(buf).c_str());
		geo::vector2 drawPos = vec;
		if (bCenter)
		{
			ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
			drawPos.x = vec.x - textSize.x / 2;
			drawPos.y = vec.y - textSize.y;
		}
		if (stroke)
		{
			window->DrawList->AddText(m_pFont, fontSize, ImVec2(drawPos.x + 1, drawPos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
			window->DrawList->AddText(m_pFont, fontSize, ImVec2(drawPos.x - 1, drawPos.y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
			window->DrawList->AddText(m_pFont, fontSize, ImVec2(drawPos.x + 1, drawPos.y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
			window->DrawList->AddText(m_pFont, fontSize, ImVec2(drawPos.x - 1, drawPos.y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		}
		window->DrawList->AddText(m_pFont, fontSize, ImVec2(drawPos.x, drawPos.y), cRender::GetU32(color), text.c_str());
	}

	void cRender::DrawLine(const geo::vector2& vecStart, const geo::vector2& vecEnd, Color color, float thickness)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		window->DrawList->AddLine(ImVec2(vecStart.x, vecStart.y), ImVec2(vecEnd.x, vecEnd.y), cRender::GetU32(color), thickness);

	}

	void cRender::DrawCircle(const geo::vector2& vecCenter, float radius, Color color, int num_seg, float thickness)
	{
		ImGuiWindow* window = ImGui::GetCurrentWindow();

		window->DrawList->AddCircle(ImVec2(vecCenter.x, vecCenter.y), radius, cRender::GetU32(color), num_seg, thickness);
	}

	void cRender::DrawSkeleton(uint64_t Mesh, FCameraCacheEntry CameraCache, Color color)
	{
		int skeleton[][4] = {

			{Head, neck_01, spine_03, spine_02},				// Ã´Ãß
			{spine_02, spine_01, pelvis, thigh_r},				// Çã¸® -> ¿ìÃø °ñ¹Ý
			{thigh_r, calf_r, foot_r},							// ¿ìÃø ´Ù¸®
			{spine_02, spine_01, pelvis, thigh_l},				// Çã¸® -> ÁÂÃø °ñ¹Ý
			{thigh_l, calf_l, foot_l},							// ÁÂÃø ´Ù¸®
			{neck_01, upperarm_l, lowerarm_l, hand_l},			// ÁÂÃø ÆÈ
			{neck_01, upperarm_r, lowerarm_r, hand_r},			// ¿ìÃø ÆÈ


			{Head, neck_01},
			{neck_01, upperarm_r, lowerarm_r, hand_r },
			{neck_01, upperarm_l, lowerarm_l, hand_l },
			{neck_01, spine_01, pelvis},
			{pelvis, thigh_r, calf_r, foot_r},
			{pelvis, thigh_l, calf_l, foot_l},
			
		};

		for (auto part : skeleton)
		{
			FVector previous{};
			for (int i = 0; i < 4; i++)
			{
				if (!part[i]) break;

				FVector current = Core::GetBoneWithRotation(Mesh, part[i]);
				if (previous.X == 0.f)
				{
					previous = current;
					continue;
				}
				FVector p1{}, c1{};
				p1 = Core::WorldToScreen(previous, CameraCache);
				c1 = Core::WorldToScreen(current, CameraCache);
				cRender::DrawLine(geo::vec2_t(p1.X, p1.Y), geo::vec2_t(c1.X, c1.Y), color, 1.0f);
				previous = current;
			}
		}
	}
}