#pragma once
#include <Windows.h>
#include "color.hpp"

struct Variables
{
	Variables() {}

	struct VISUALS
	{
		bool ESP;
		bool TeamESP;
		bool Skeleton;
		bool HeadSkeleton;
		bool vis_check;
		bool Snapline;
		float distance = 0.f;
	} visuals;

	struct AIMBOT
	{
		bool Aimbot;
		float smooth;
		bool SilentAim;
		bool no_recoil;

		bool fov;
		float fov_size;

	} aimbot;

	struct MISC
	{
		bool max_stamina;

		bool show_fps;

	} misc;

};

inline Variables Vars;