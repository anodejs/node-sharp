#pragma once

#include <node.h>
#include <v8.h>

void LoadAssembly();

// this header contains no managed references, so it can act as a shim
// from the addon to the managed assembly loading classes
class SharpLibHelper
{
public:
    static v8::Handle<v8::Value> Instance(const v8::Arguments& args);
};
