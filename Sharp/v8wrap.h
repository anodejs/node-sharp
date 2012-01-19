// Inserted class to allow the v8sharp structures to share
// object constructs with the rest of this (nodeSharp) project.
#pragma once

#include <v8.h>
#include <vcclr.h>

class V8ObjectWrapper 
{
public:

	static v8::Handle<v8::Value> Wrap(System::Object^ obj);

	static System::Object^ Unwrap(v8::Handle<v8::Value> value);
    
    static bool IsObjectWrapper(v8::Handle<v8::Value> value);
};