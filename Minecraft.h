#pragma once

#include <Windows.h>

class Minecraft
{
public:
	static HWND getWindow();

private:
	static bool checkSupport(HWND window, DWORD flag);
	static BOOL CALLBACK enumWindowsProc(HWND window, LPARAM param);
	static BOOL CALLBACK enumChildWindowsProc(HWND window, LPARAM param);
};