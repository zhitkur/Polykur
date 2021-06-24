#pragma once
#include <cstdint>

namespace mem
{
	static uintptr_t dwGameBase = (uintptr_t)GetModuleHandle(NULL);

	template<class T>
	static T RPM(uintptr_t address)
	{
		__try
		{
			return *(T*)(address);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return T{};
		}
	}

	template<class T>
	static void WPM(uintptr_t address, T value)
	{
		__try
		{
			*(T*)(address) = value;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}
}