#include "executor.hpp"
#include "core.hpp"
#include "structure.hpp"
#include "render.hpp"

#pragma warning(disable : 4244)

namespace polykur
{
	void function::Executor()
	{
		__try
		{
			Core::Update();

			ESP();
			NoRecoil();
			//Aimbot();
		}
		__except(EXCEPTION_EXECUTE_HANDLER) {}
	}

	void function::ESP()
	{
		// Draw Watermark
		cRender::DrawString(18, geo::vec2_t(cRender::Width / 2 - 50, 100), Color(0xFFDC143C), false, false, E("zhitkur :)"));
		//cRender::DrawString(18, geo::vec2_t(20, 100), Color::White, false, true, "GameBase: 0x%p", mem::dwGameBase);
		// Draw ShowFPS
		if (Vars.misc.show_fps)
			cRender::DrawString(18, geo::vec2_t(10, 10), Color::White, false, true, E("FPS: %i | %.fms"), G::FrameRate(), 1000.f / G::FrameRate());

		// Draw FOV
		if (Vars.aimbot.fov)
			cRender::DrawCircle(geo::vec2_t(cRender::Width / 2, cRender::Height / 2), Vars.aimbot.fov_size,
				g_Options.fov_color, 128, 1.0);

		if (Vars.visuals.ESP)
		{
			FCameraCacheEntry CameraCache = mem::RPM<FCameraCacheEntry>(Core::PlayerCameraManager + Core::CameraCache);
			for (int i = 0; i < Core::ActorCounts; i++)
			{
				// Actor == Players[i]
				uint64_t Actor = mem::RPM<uint64_t>(Core::Actors + i * 0x8);
				// Actor == Core::AcknowledgedPawn -> LocalPlayer
				if (!Actor || Actor == Core::AcknowledgedPawn) continue;

				uint64_t PlayerState = mem::RPM<uint64_t>(Actor + Core::PlayerState);
				uint64_t Mesh = mem::RPM<uint64_t>(Actor + Core::Mesh);

				// Get TeamId - PlayerState + Team Offset
				uint8_t TeamId = mem::RPM<uint8_t>(PlayerState + Core::Team);

				// Get Player Position 
				FVector root = Core::GetBoneWithRotation(Mesh, 0); 
				FVector head = Core::GetBoneWithRotation(Mesh, 6);
				FVector headspos = Core::WorldToScreen(head, CameraCache);
				FVector spos = Core::WorldToScreen(root, CameraCache);

				// Get PlayerName - PlayerState + PlayerName Offset
				FString PlayerName = mem::RPM<FString>(mem::RPM<uint64_t>(PlayerState + Core::PlayerName));
				std::string PlayerNameStr = Hexto::WStringToUTF8(PlayerName.szChar);
				
				// Enemy ESP
				if (PlayerNameStr.empty() == NULL && Core::CheckPlayerAlive(Actor) && Core::LocalTeam != TeamId)
				{
					// Draw PlayerName, Distance
					cRender::DrawString(14, geo::vec2_t(spos.X, spos.Y + 20), Color(0xFF98bf86), true, true, E("%s"), PlayerNameStr.c_str());
					cRender::DrawString(14, geo::vec2_t(spos.X, spos.Y + 35), Color::White, true, true, E("[%.fm]"), Vars.visuals.distance);

					// Draw Skeleton
					if (Vars.visuals.Skeleton)
					{
						//cRender::DrawSkeleton(Mesh, CameraCache, cRender::espcolor);
						cRender::DrawSkeleton(Mesh, CameraCache, Color(0xFF00FF7F));
						cRender::DrawCircle(geo::vec2_t(headspos.X + 2, headspos.Y - 2), 3, Color::Red, 8, 1.5);
					}

					// Draw Snapline
					if (Vars.visuals.Snapline)
						cRender::DrawLine(geo::vec2_t(cRender::Width / 2, cRender::Height), geo::vec2_t(spos.X, spos.Y + 38), Color::White, 0.5f);
				}

				// TEAM ESP
				else if (PlayerNameStr.empty() == NULL && Core::CheckPlayerAlive(Actor) && Core::LocalTeam == TeamId && Vars.visuals.TeamESP)
				{
					// Draw PlayerName, Distance
					cRender::DrawString(14, geo::vec2_t(spos.X, spos.Y + 20), Color(0xFF808080), true, true, E("%s"), PlayerNameStr.c_str());
					cRender::DrawString(14, geo::vec2_t(spos.X, spos.Y + 35), Color(0xFF808080), true, true, E("[%.fm]"), Vars.visuals.distance);

					// Draw Skeleton
					if (Vars.visuals.Skeleton)
						cRender::DrawSkeleton(Mesh, CameraCache, Color(0xFFE9967A));

					// Draw Snapline
					//if (Vars.visuals.Snapline)
					//	cRender::DrawLine(geo::vec2_t(cRender::Width / 2, cRender::Height), geo::vec2_t(spos.X, spos.Y + 38), Color(0xFF696969), 0.5f);
				}


			}
		}
	}

	void function::NoRecoil()
	{
		if (Vars.aimbot.no_recoil)
		{
			uintptr_t GunBaseAddress = mem::dwGameBase + 0x057D6F30;

			uintptr_t NoRecoil = Core::GetPtrAddress(GunBaseAddress, { 0x190, 0x30, 0x250, 0x190, 0x8, 0x120, 0x2D0 });

			mem::WPM<float>(NoRecoil, 0.0f);	
		}

		else
		{
			uintptr_t GunBaseAddress = mem::dwGameBase + 0x057D6F30;

			uintptr_t NoRecoil = Core::GetPtrAddress(GunBaseAddress, { 0x190, 0x30, 0x250, 0x190, 0x8, 0x120, 0x2D0 });

			mem::WPM<float>(NoRecoil, 16.0f);
		}

	}

	void function::Aimbot()
	{
		if (Vars.aimbot.Aimbot)
		{
			uintptr_t GunBaseAddress = mem::dwGameBase + 0x57EE5F0;

			uintptr_t pitch_addr = Core::GetPtrAddress(GunBaseAddress, { 0x38, 0x88, 0x350, 0x70, 0x4EC });
			uintptr_t yaw_addr = Core::GetPtrAddress(GunBaseAddress, { 0x38, 0x88, 0x350, 0x70, 0x4E4 });

			float pitch = mem::RPM<float>(pitch_addr);
			float yaw = mem::RPM<float>(yaw_addr);

			// Aimbot Stuff
		}
	}
}