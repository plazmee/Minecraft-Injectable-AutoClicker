#include "Minecraft.h"

HWND Minecraft::getWindow()
{
	HWND window = nullptr;
	EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&window));
	return window;
}

bool Minecraft::checkSupport(HWND window, DWORD flag)
{
	PIXELFORMATDESCRIPTOR pfd{};

	HDC hDC = GetDC(window);

	if (!hDC)
	{
		return false;
	}

	int format = GetPixelFormat(hDC);

	if (!format)
	{
		return false;
	}

	if (!DescribePixelFormat(hDC, format, sizeof(pfd), &pfd))
	{
		return false;
	}

	return pfd.dwFlags & flag;
}

BOOL CALLBACK Minecraft::enumWindowsProc(HWND window, LPARAM param)
{
	DWORD processId;
	GetWindowThreadProcessId(window, &processId);

	if (GetCurrentProcessId() != processId)
	{
		return TRUE;
	}

	if (!IsWindowVisible(window))
	{
		return TRUE;
	}

	if (checkSupport(window, PFD_SUPPORT_OPENGL))
	{
		*reinterpret_cast<HWND*>(param) = window;
		return FALSE;
	}

	EnumChildWindows(window, enumChildWindowsProc, param);

	return TRUE;
}

BOOL CALLBACK Minecraft::enumChildWindowsProc(HWND window, LPARAM param)
{
	if (checkSupport(window, PFD_SUPPORT_OPENGL))
	{
		*reinterpret_cast<HWND*>(param) = window;
		return FALSE;
	}

	return TRUE;
}