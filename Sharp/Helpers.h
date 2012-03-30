#pragma once

#include <uv.h>
#include <v8.h>
#include <vcclr.h>
#include <node.h>

class Helpers
{
public:
    static System::String^ GetError(System::Exception^ e);
    static array<System::Object^>^ ConvertArguments(const v8::Arguments & args, int startIndex, array<System::Reflection::ParameterInfo^>^ types);
};
