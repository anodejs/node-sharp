#pragma once

#include <node.h>
#include <v8.h>
#include <vcclr.h>


class WrapBase : node::ObjectWrap
{
private:
    gcroot<System::String^> _searchBase;

public:
    WrapBase();

    static v8::Handle<v8::Value> ListAssemblies(const v8::Arguments& args);

    static v8::Handle<v8::Value> LoadAssembly(const v8::Arguments& args);

    static v8::Handle<v8::Value> New();
};