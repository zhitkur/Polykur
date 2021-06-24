#include "config.hpp"
#include "xor.hpp"
#include "var.hpp"
#include "color.hpp"
#include <Windows.h>
#include <ShlObj.h>


void CConfig::Setup()
{
	// VISUALS
	SetupValue(Vars.visuals.ESP, true, E("Visuals"), E("ESP"));
	SetupValue(Vars.visuals.TeamESP, false, E("Visuals"), E("TeamESP"));
	SetupValue(Vars.visuals.Skeleton, true, E("Visuals"), E("Skeleton"));
	SetupValue(Vars.visuals.vis_check, false, E("Visuals"), E("vis_check"));
	SetupValue(Vars.visuals.Snapline, false, E("Visuals"), E("Snapline"));
	SetupValue(Vars.visuals.distance, 0.f, E("Visuals"), E("distance"));

	// AIMBOT
	SetupValue(Vars.aimbot.Aimbot, true, E("Aimbot"), E("Aimbot"));
	SetupValue(Vars.aimbot.SilentAim, false, E("Aimbot"), E("SlientAim"));
	SetupValue(Vars.aimbot.smooth, 5.f, E("Aimbot"), E("smooth"));
	SetupValue(Vars.aimbot.no_recoil, false, E("Aimbot"), E("no_recoil"));
	SetupValue(Vars.aimbot.fov, true, E("Aimbot"), E("fov"));
	SetupValue(Vars.aimbot.fov_size, 150.f, E("Aimbot"), E("fov_size"));

	// MISC
	SetupValue(Vars.misc.max_stamina, false, E("Misc"), E("max_stamina"));

	SetupValue(Vars.misc.show_fps, true, E("Misc"), E("show_fps"));



}

void CConfig::SetupValue(int& value, int def, std::string category, std::string name)
{
	value = def;
	ints.push_back(new ConfigValue< int >(category, name, &value));
}

void CConfig::SetupValue(float& value, float def, std::string category, std::string name)
{
	value = def;
	floats.push_back(new ConfigValue< float >(category, name, &value));
}

void CConfig::SetupValue(bool& value, bool def, std::string category, std::string name)
{
	value = def;
	bools.push_back(new ConfigValue< bool >(category, name, &value));
}


void CConfig::Save()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + E("\\polykur\\");
		file = std::string(path) + E("\\polykur\\polykur.ini");
	}

	CreateDirectory(folder.c_str(), NULL);

	for (auto value : ints)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : floats)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), std::to_string(*value->value).c_str(), file.c_str());

	for (auto value : bools)
		WritePrivateProfileString(value->category.c_str(), value->name.c_str(), *value->value ? E("true") : E("false"), file.c_str());
}

void CConfig::Load()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + E("\\polykur\\");
		file = std::string(path) + E("\\polykur\\polykur.ini");
	}

	CreateDirectory(folder.c_str(), NULL);

	char value_l[32] = { '\0' };

	for (auto value : ints)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atoi(value_l);
	}

	for (auto value : floats)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = atof(value_l);
	}

	for (auto value : bools)
	{
		GetPrivateProfileString(value->category.c_str(), value->name.c_str(), "", value_l, 32, file.c_str());
		*value->value = !strcmp(value_l, E("true"));
	}
}

void CConfig::Clear()
{
	//// VISUALS
	//SetupValue(Vars.visuals.ESP, true, E("Visuals"), E("ESP"));
	//SetupValue(Vars.visuals.Skeleton, true, E("Visuals"), E("Skeleton"));
	//SetupValue(Vars.visuals.vis_check, false, E("Visuals"), E("vis_check"));
	//SetupValue(Vars.visuals.Snapline, false, E("Visuals"), E("Snapline"));

	//// AIMBOT
	//SetupValue(Vars.aimbot.Aimbot, true, E("Aimbot"), E("Aimbot"));
	//SetupValue(Vars.aimbot.SilentAim, false, E("Aimbot"), E("SlientAim"));
	//SetupValue(Vars.aimbot.smooth, 5.f, E("Aimbot"), E("smooth"));
	//SetupValue(Vars.aimbot.fov, true, E("Aimbot"), E("fov"));
	//SetupValue(Vars.aimbot.fov_size, 150.f, E("Aimbot"), E("fov_size"));

	//// MISC
	//SetupValue(Vars.misc.max_stamina, false, E("Misc"), E("max_stamina"));

	//SetupValue(Vars.misc.show_fps, true, E("Misc"), E("show_fps"));
}

CConfig* Config = new CConfig();