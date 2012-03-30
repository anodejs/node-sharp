#include "v8wrap.h"
#include "WrapInstance.h"

v8::Handle<v8::Value> V8ObjectWrapper::Wrap(System::Object^ obj)
{
    if (obj == nullptr)
    {
        return v8::Null();
    }

    return WrapInstance::New(obj->GetType(), obj);
}

System::Object^ V8ObjectWrapper::Unwrap(v8::Handle<v8::Value> value)
{
    return WrapInstance::Unwrap(value);
}

bool V8ObjectWrapper::IsObjectWrapper(v8::Handle<v8::Value> value)
{
    return WrapInstance::IsWrapped(value);
}
