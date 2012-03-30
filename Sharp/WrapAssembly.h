#pragma once

#include <v8.h>
#include <vcclr.h>
#include <node.h>

class WrapAssembly : public node::ObjectWrap
{
private:
    gcroot<System::Reflection::Assembly^> _assembly;

    WrapAssembly(System::Reflection::Assembly^ a);

public:
    
    static v8::Handle<v8::Value> AssemblyName(const v8::Arguments& args);
    static v8::Handle<v8::Value> ListTypes(const v8::Arguments& args);
    static v8::Handle<v8::Value> CreateInstance(const v8::Arguments& args);
    static v8::Handle<v8::Value> StaticInstance(const v8::Arguments& args);

    static v8::Handle<v8::Value> New(System::Reflection::Assembly ^ a);
};