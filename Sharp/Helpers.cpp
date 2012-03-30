#include "Helpers.h"
#include "v8value.h"
#include "MatchType.h"

using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Reflection;

System::String^ Helpers::GetError(System::Exception^ e)
{
    StringBuilder^ sb = gcnew StringBuilder();

    while(e != nullptr)
    {
        sb->AppendLine(e->Message);
        sb->AppendLine("----------");
        e = e->InnerException;
    }

    return sb->ToString();
}

array<System::Object^>^ Helpers::ConvertArguments(const v8::Arguments& args, int startIndex, array<System::Reflection::ParameterInfo^>^ types)
{
    List<System::Object^>^ argList = gcnew List<System::Object^>();
    if (args.Length() > startIndex)
    {
        for(int i=startIndex; i < args.Length(); i++)
        {
            argList->Add(v8sharp::V8Interop::FromV8(args[i]));
        }
    }
    
    array<System::Object^>^ result = MatchType::AdjustArguments(argList->ToArray(), types);
    
    return result;
}
