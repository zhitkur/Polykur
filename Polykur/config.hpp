#pragma once
#include <Windows.h>
#include <cstdint>
#include <atomic>
#include <ctime>
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>

#include "render.hpp"
#include "color.hpp"
#include "var.hpp"

#define A( s ) #s
#define OPTION(type, var, val) Var<type> var = {A(var), val}

namespace polykur
{
	inline bool IsKeyPressed(std::uint16_t key)
	{
		if (GetForegroundWindow() == cRender::GameWnd)
		{
			if (GetAsyncKeyState(key) & 0x8000)
			{
				return true;
			}
		}

		return false;
	}

	inline time_t g_Expire{};
	inline std::atomic_bool g_Running = true;

	static bool g_MenuOpened = true;
	static int g_TabCount = 0;
}

template <typename T = bool>
class Var {
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) {
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T* () { return &*value; }
	operator T() const { return *value; }
	//operator T*() const { return value; }
};

class Options
{
public:
	// MISC
	OPTION(Color, fov_color, Color(255, 255, 255));
	OPTION(Color, text_color, Color(72, 228, 34));
};

inline Options g_Options;


template< typename T >
class ConfigItem
{
	std::string category, name;
	T* value;
public:
	ConfigItem(std::string category, std::string name, T* value)
	{
		this->category = category;
		this->name = name;
		this->value = value;
	}
};

template< typename T >
class ConfigValue
{
public:
	ConfigValue(std::string category_, std::string name_, T* value_)
	{
		category = category_;
		name = name_;
		value = value_;
	}

	std::string category, name;
	T* value;
};

class CConfig
{
protected:
	std::vector< ConfigValue< int >* > ints;
	std::vector< ConfigValue< bool >* > bools;
	std::vector< ConfigValue< float >* > floats;
private:
	void SetupValue(int&, int, std::string, std::string);
	void SetupValue(bool&, bool, std::string, std::string);
	void SetupValue(float&, float, std::string, std::string);
public:
	CConfig()
	{
		Setup();
		Load();
	}

	void Setup();

	void Save();
	void Load();

	void Clear();
};

extern CConfig* Config;