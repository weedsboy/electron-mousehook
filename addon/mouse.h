#ifndef SIPAPI_H
#define SIPAPI_H

#include <napi.h>

Napi::Value StartMouseHook(const Napi::CallbackInfo& info);
void StopMouseHook(const Napi::CallbackInfo& info);

#endif
