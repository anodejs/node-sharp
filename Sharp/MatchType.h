#pragma once

#include <vcclr.h>

class MatchType
{
public:
    static array<System::Object^>^ AdjustArguments(array<System::Object^>^ argumentList, array<System::Reflection::ParameterInfo^>^ types);

    static System::Object^ AdjustObject(System::Object^ obj, System::Type^ type);
};