#ifndef SIPAPI_H
#define SIPAPI_H

#include <napi.h>
#include <Windows.h>

Napi::Value StartMouseHook(const Napi::CallbackInfo& info);
void StopMouseHook(const Napi::CallbackInfo& info);

BOOL OpenMouseHook(HWND hMainWnd, UINT mouseMsg);
void CloseMouseHook();

#endif
