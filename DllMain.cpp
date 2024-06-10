

//	DllMain.cpp

	#include "DllMain.h"

	#include <iostream>
	using namespace std;

	int mouseX = 0;
	int mouseY = 0;

	bool state = true;

	HHOOK hookHandle = nullptr;
	HANDLE threadHandle = nullptr;

	DWORD WINAPI clicker(LPVOID param)
	{
		HWND window = reinterpret_cast<HWND>(param);

		while (state and GetAsyncKeyState(VK_LBUTTON) < 0)
		{
			PostMessage(window, WM_LBUTTONDOWN, 0, MAKELPARAM(mouseX, mouseY));
			PostMessage(window, WM_LBUTTONUP, 0, MAKELPARAM(mouseX, mouseY));

			Sleep(5);

			if (state and GetAsyncKeyState(VK_LBUTTON) < 0 and GetAsyncKeyState(VK_RBUTTON) < 0)
			{
				PostMessage(window, WM_RBUTTONDOWN, 0, MAKELPARAM(mouseX, mouseY));
				PostMessage(window, WM_RBUTTONUP, 0, MAKELPARAM(mouseX, mouseY));

				Sleep(5);
			}
		}

		if (GetAsyncKeyState(VK_RBUTTON) < 0)
		{
			PostMessage(window, WM_RBUTTONDOWN, 0, MAKELPARAM(mouseX, mouseY));
		}

		if (!CloseHandle(threadHandle))
		{
			return GetLastError();
		}

		threadHandle = nullptr;
		return 0;
	}

	LRESULT CALLBACK messageHandler(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode < 0)
		{
			return CallNextHookEx(hookHandle, nCode, wParam, lParam);
		}

		MOUSEHOOKSTRUCT* data = reinterpret_cast<MOUSEHOOKSTRUCT*>(lParam);

		switch (wParam)
		{
			case WM_LBUTTONDOWN:
			{
				if (!state or threadHandle)
				{
					return CallNextHookEx(hookHandle, nCode, wParam, lParam);
				}

				threadHandle = CreateThread(nullptr, 0, clicker, reinterpret_cast<LPVOID>(data->hwnd), 0, nullptr);
			}

			break;

			case WM_MOUSEMOVE:
			{
				POINT position = data->pt;

				if (ScreenToClient(data->hwnd, &position))
				{
					mouseX = position.x;
					mouseY = position.y;
				}
			}

			break;

			case WM_MBUTTONUP:
			{
				state = !state;
			}
		}

		return CallNextHookEx(hookHandle, nCode, wParam, lParam);
	}

	DWORD WINAPI messageDispatcher(LPVOID thread)
	{
		hookHandle = SetWindowsHookEx(WH_MOUSE, messageHandler, GetModuleHandle(nullptr), *reinterpret_cast<DWORD*>(thread));

		if (!hookHandle)
		{
			return GetLastError();
		}

		MSG message{};

		while (GetMessage(&message, 0, 0, 0) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		return 0;
	}

	BOOL APIENTRY DllMain(HINSTANCE handle, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
			case DLL_PROCESS_ATTACH:
			{
				HWND window = Minecraft::getWindow();

				if (!window)
				{
					return FALSE;
				}

				static DWORD thread = GetWindowThreadProcessId(window, nullptr);

				if (!thread)
				{
					return FALSE;
				}

				if (!CreateThread(nullptr, 0, messageDispatcher, reinterpret_cast<LPVOID>(&thread), 0, nullptr))
				{
					return FALSE;
				}

				return TRUE;
			}

			break;

			case DLL_PROCESS_DETACH:
			{
				return UnhookWindowsHookEx(hookHandle);
			}
		}

		return TRUE;
	}