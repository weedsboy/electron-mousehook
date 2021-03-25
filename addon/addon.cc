#include <napi.h>
#include "mouse.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) 
{ 
    exports.Set(Napi::String::New(env, "StartMouseHook"), Napi::Function::New(env, StartMouseHook));
    exports.Set(Napi::String::New(env, "StopMouseHook"), Napi::Function::New(env, StopMouseHook));

    return exports;
}

NODE_API_MODULE(MouseHook, InitAll)