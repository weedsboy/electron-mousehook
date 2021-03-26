#include <napi.h>
#include "mouse.h"

Napi::Value StartMouseHook(const Napi::CallbackInfo& info)
{
    Napi::Number hwndValue = info[0].As<Napi::Number>();
    int eventMsg = ::RegisterWindowMessage("MY MOUSE HOOK MESSAGE");

    HWND hwnd = (HWND)hwndValue.Int32Value();

    if(!::IsWindow(hwnd))
    {
        return Napi::Number::New(info.Env(), 0);
    }

    BOOL result = OpenMouseHook(hwnd, eventMsg);
    return Napi::Number::New(info.Env(), result ? eventMsg : 0);
}

void StopMouseHook(const Napi::CallbackInfo& info)
{
    CloseMouseHook();
}

HWND g_hMainWnd = nullptr;
UINT g_mouseMsg = 0;
HHOOK g_hMouseHook = nullptr;

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

	if (wParam == WM_LBUTTONDOWN || wParam == WM_NCLBUTTONDOWN || wParam == WM_LBUTTONUP ||
		wParam == WM_NCLBUTTONUP || wParam == WM_MOUSEMOVE)
	{
		MSLLHOOKSTRUCT* data = (MSLLHOOKSTRUCT*)lParam;
		POINT point = data->pt;
		::PostMessage(g_hMainWnd, g_mouseMsg, wParam, MAKELONG(point.x, point.y));
	}

	return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

BOOL OpenMouseHook(HWND hMainWnd, UINT mouseMsg)
{
	g_hMainWnd = hMainWnd;
	g_mouseMsg = mouseMsg;

	EnableDebugPrivilege();

	g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, 0, 0);
	if (g_hMouseHook == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

void CloseMouseHook()
{
	if (g_hMouseHook)
	{
		UnhookWindowsHookEx(g_hMouseHook);
		g_hMouseHook = nullptr;
	}
}