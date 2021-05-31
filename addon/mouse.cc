#include <napi.h>
#include "mouse.h"

#ifdef WIN32
#include <Windows.h>

UINT g_mouseMsg = 0;
HHOOK g_hMouseHook = nullptr;
HHOOK g_hKeyboardHook = nullptr;
BOOL g_isMouseHookOpen = FALSE;
std::vector<HWND> g_wndList;

BOOL EnableDebugPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return(FALSE);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	if (GetLastError() != ERROR_SUCCESS)
		return FALSE;

	return TRUE;
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode < 0)  // do not process the message 
		return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

	if (wParam == WM_LBUTTONDOWN || wParam == WM_NCLBUTTONDOWN || 
	    wParam == WM_LBUTTONUP || wParam == WM_NCLBUTTONUP || 
		wParam == WM_RBUTTONDOWN || wParam == WM_NCRBUTTONDOWN || 
		wParam == WM_RBUTTONUP || wParam == WM_NCRBUTTONUP || 
		wParam == WM_MOUSEMOVE)
	{
		MSLLHOOKSTRUCT* data = (MSLLHOOKSTRUCT*)lParam;
		POINT point = data->pt;

		std::vector<HWND>::iterator it = g_wndList.begin();
		for (; it != g_wndList.end(); it++)
		{
			::PostMessage(*it, g_mouseMsg, wParam, MAKELONG(point.x, point.y));
		}
	}

	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	if (nCode < 0)  // do not process the message 
		return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

	KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;
	if (nCode == HC_ACTION && pkbhs->vkCode == VK_SPACE)
	{
		std::vector<HWND>::iterator it = g_wndList.begin();
		for (; it != g_wndList.end(); it++)
		{
			::PostMessage(*it, g_mouseMsg, wParam, lParam);
		}
	}

	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

BOOL OpenMouseHook(HWND hwnd)
{
	std::vector<HWND>::iterator it = std::find(g_wndList.begin(), g_wndList.end(), hwnd);
	if (it == g_wndList.end())
	{
		g_wndList.push_back(hwnd);
	}

	if (!g_isMouseHookOpen)
	{
		EnableDebugPrivilege();

		g_mouseMsg = ::RegisterWindowMessage("MY MOUSE HOOK MESSAGE");
		g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, 0, 0);
		g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, 0, 0);
		if (g_hMouseHook == NULL)
		{
			g_isMouseHookOpen = FALSE;
			return FALSE;
		}

		g_isMouseHookOpen = TRUE;
		return g_mouseMsg;
	}

	return g_mouseMsg;
}

void CloseMouseHook(HWND hWnd)
{
	std::vector<HWND>::iterator it = g_wndList.begin();
	for (; it != g_wndList.end(); )
	{
		if (hWnd == *it)
		{
			it = g_wndList.erase(it);
		}
		else
		{
			it++;
		}
	}

	if (g_wndList.size() <= 0)
	{
		g_isMouseHookOpen = FALSE;
		if (g_hMouseHook)
		{
			UnhookWindowsHookEx(g_hMouseHook);
			g_hMouseHook = nullptr;
		}

		if (g_hKeyboardHook)
		{
			UnhookWindowsHookEx(g_hKeyboardHook);
			g_hKeyboardHook = nullptr;
		}
	}
}

Napi::Value StartMouseHook(const Napi::CallbackInfo& info)
{
	Napi::Number hwndValue = info[0].As<Napi::Number>();
	HWND hwnd = (HWND)hwndValue.Int32Value();

	if (!::IsWindow(hwnd))
	{
		return Napi::Number::New(info.Env(), 0);
	}

	BOOL result = OpenMouseHook(hwnd);
	return Napi::Number::New(info.Env(), result);
}

void StopMouseHook(const Napi::CallbackInfo& info)
{
	Napi::Number hwndValue = info[0].As<Napi::Number>();
	HWND hwnd = (HWND)hwndValue.Int32Value();

	CloseMouseHook(hwnd);
}
#endif