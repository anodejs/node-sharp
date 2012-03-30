#include "WrapAssembly.h"
#include "WrapInstance.h"
#include "v8value.h"
#include "Helpers.h"

using namespace System::Text;
using namespace System::Collections::Generic;
using namespace System::Reflection;
using namespace v8;

WrapAssembly::WrapAssembly(Assembly^ a)
{
    _assembly = a;
}

Handle<Value> WrapAssembly::AssemblyName(const Arguments& args)
{
    HandleScope scope;
    WrapAssembly* self = node::ObjectWrap::Unwrap<WrapAssembly>(args.This());

    Handle<Value> result = v8sharp::V8Interop::ToV8(self->_assembly->FullName);

    return scope.Close(result);
}

Handle<Value> WrapAssembly::ListTypes(const Arguments& args)
{
    HandleScope scope;
    WrapAssembly* self = node::ObjectWrap::Unwrap<WrapAssembly>(args.This());

    List<System::String^>^ types = gcnew List<System::String^>();

    for each(Type^ t in self->_assembly->GetTypes())
    {
        types->Add(t->FullName);
    }

    Handle<Value> result = v8sharp::V8Interop::ToV8(types->ToArray());

    return scope.Close(result);
}

Handle<Value> WrapAssembly::CreateInstance(const Arguments& args)
{
    if (args.Length() < 1 || !args[0]->IsString()) {
        return ThrowException(v8::Exception::TypeError(
            v8::String::New("First argument must be a string, for class name.")));
    }

    HandleScope scope;
    WrapAssembly* self = node::ObjectWrap::Unwrap<WrapAssembly>(args.This());

    System::String^ path =  (System::String^)v8sharp::V8Interop::FromV8(args[0]);

    try
    {
        Type^ t = self->_assembly->GetType(path);

        if (t == nullptr)
        {
            throw gcnew System::NotSupportedException("Class: '" + path + "' was not found");
        }

        array<ConstructorInfo^>^ cInfoList = t->GetConstructors();

        List<ConstructorInfo^>^ matches = gcnew List<ConstructorInfo^>();
        int otherArgCount = args.Length() - 1;
        for (int i = 0; i < cInfoList->Length; i++)
        {
            ConstructorInfo^ ci = cInfoList[i];
            array<System::Reflection::ParameterInfo^>^ params = ci->GetParameters();
            if (params->Length == otherArgCount)
            {
                matches->Add(ci);
            }
        }

        if (matches->Count == 0)
        {
            throw gcnew System::NotSupportedException("Class: '" + path + "' does not have a constructor with " + otherArgCount + " arguments.");
        }
        else if (matches->Count > 1)
        {
            Console::WriteLine("Node-Sharp Warning: Constructor matching by length found multiple options.");
        }

        array<System::Object^>^ argList = Helpers::ConvertArguments(args, 1, matches[0]->GetParameters());
        System::Object^ result = matches[0]->Invoke(argList);

        return scope.Close(WrapInstance::New(t, result));
    }
    catch (System::Exception^ e)
    {
        v8::Handle<v8::Value> str =  v8sharp::V8Interop::ToV8(Helpers::GetError(e));
        v8::Handle<v8::String> err = v8::Handle<v8::String>::Cast(str);

        return ThrowException(v8::Exception::Error(err));
    }
}

Handle<Value> WrapAssembly::StaticInstance(const Arguments& args)
{
    if (args.Length() < 1 || !args[0]->IsString()) {
        return ThrowException(v8::Exception::TypeError(
            v8::String::New("First argument must be a string, for class name.")));
    }

    HandleScope scope;
    WrapAssembly* self = node::ObjectWrap::Unwrap<WrapAssembly>(args.This());

    System::String^ path =  (System::String^)v8sharp::V8Interop::FromV8(args[0]);

    try
    {
        Type^ t = self->_assembly->GetType(path);

        if (t == nullptr)
        {
            throw gcnew System::NotSupportedException("Class: '" + path + "' was not found");
        }

        return scope.Close(WrapInstance::New(t, nullptr));
    }
    catch (System::Exception^ e)
    {
        v8::Handle<v8::Value> str =  v8sharp::V8Interop::ToV8(Helpers::GetError(e));
        v8::Handle<v8::String> err = v8::Handle<v8::String>::Cast(str);

        return ThrowException(v8::Exception::Error(err));
    }
}

Handle<v8::Value> WrapAssembly::New(Assembly ^ a)
{
    HandleScope scope;

    Local<ObjectTemplate> oTemplate = ObjectTemplate::New();
    oTemplate->SetInternalFieldCount(1);
    Local<v8::Object> result = oTemplate->NewInstance();

    WrapAssembly* self = new WrapAssembly(a);
    self->Wrap(result);

    result->Set(v8::String::NewSymbol("assembly"), v8sharp::V8Interop::ToV8(self->_assembly->FullName));
    result->Set(v8::String::NewSymbol("types"), FunctionTemplate::New(ListTypes)->GetFunction() );
    result->Set(v8::String::NewSymbol("new"), FunctionTemplate::New(CreateInstance)->GetFunction() );
    result->Set(v8::String::NewSymbol("static"), FunctionTemplate::New(StaticInstance)->GetFunction() );

    return scope.Close(result);
}