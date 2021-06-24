#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "structure.hpp"
#include "xor.hpp"


namespace G
{
	inline HMODULE g_Module{};
	void ThreadStuff(bool bSuspend);
	int FrameRate();
	std::string random_string(const int len);
}


namespace Hexto
{
	std::wstring MBytesToWString(const char* lpcszString);
	std::string WStringToMBytes(const wchar_t* lpwcszWString);
	std::wstring UTF8ToWString(const char* lpcszString);
	std::string WStringToUTF8(const wchar_t* lpwcszWString);
	std::string readString(const DWORD64 dwPtr);

	std::vector<char> HexToBytes(const std::string& hex);
	std::string BytesToString(unsigned char* data, int len);
	std::vector<std::string> Split(const std::string& str, const char* delim);
}