#pragma once

#include <uv.h>
#include <v8.h>
#include <vcclr.h>
#include <node.h>

class WrapInstance;

class Baton {
public:
    uv_work_t request;
    v8::Persistent<v8::Function> callback;

    // Enough detail to make the call
    WrapInstance* caller;
    gcroot<System::Reflection::MethodInfo^> method;
    gcroot<array<System::Object^>^> args;

    // error details
    bool error;
    gcroot<System::String^> error_message;

    // result details
    gcroot<System::Object^> result;
};

class WrapInstance : node::ObjectWrap
{
private:
    gcroot<System::Object^> _instance;
    gcroot<System::Type^> _type;
    bool _isStatic;

    WrapInstance(System::Type^ t, System::Object^ i);

public:

    static v8::Handle<v8::Value> GetClassName(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> ListMethods(const v8::Arguments& args);

    static v8::Handle<v8::Value> CallMethod(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> Async(const v8::Arguments& args);
    
    static v8::Handle<v8::Value> New(System::Type^ t, System::Object^ o);
    static bool IsWrapped(v8::Handle<v8::Value> value);
    static System::Object^ Unwrap(v8::Handle<v8::Value> value);

private:
    // this runs on the worker thread and should not callback or interact with node/v8 in any way
    static void StartAsync(uv_work_t* req);

    // this runs on the main thread and can call back into the JavaScript
    static void AfterAsync(uv_work_t *req); 

};