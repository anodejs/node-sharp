#include "WrapBase.h"
#include "v8value.h"
#include "WrapAssembly.h"

using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Reflection;

using namespace v8;

WrapBase::WrapBase()
{
    _searchBase = System::Environment::CurrentDirectory;
}

Handle<Value> WrapBase::ListAssemblies(const v8::Arguments& args)
{
    HandleScope scope;
    WrapBase* self = node::ObjectWrap::Unwrap<WrapBase>(args.This());

    DirectoryInfo^ d = gcnew DirectoryInfo(self->_searchBase);

    List<System::String^>^ names = gcnew List<System::String^>();
    for each(FileInfo^ f in d->EnumerateFiles("*.dll"))
    {
        names->Add(f->Name);
    }

    return scope.Close( v8sharp::V8Interop::ToV8(names->ToArray()) );
}

Handle<Value> WrapBase::LoadAssembly(const v8::Arguments& args)
{
    if (args.Length() < 1 || !args[0]->IsString()) {
        return ThrowException(v8::Exception::TypeError(
            v8::String::New("First argument must be a string, for dll name.")));
    }

    HandleScope scope;
    WrapBase* self = node::ObjectWrap::Unwrap<WrapBase>(args.This());

    System::String^ path =  safe_cast<System::String^>(v8sharp::V8Interop::FromV8(args[0]) );

    try
    {
        DirectoryInfo^ d = gcnew DirectoryInfo(self->_searchBase);
        array<FileInfo^>^ files = d->GetFiles(path);

        if (files->Length <= 0)
        {
            throw gcnew System::Exception(System::String::Format("File {0} not found", path));
        }

        FileInfo^ f = files[0];

        Assembly^ a = Assembly::LoadFile(f->FullName);

        return scope.Close( WrapAssembly::New(a) );
    }
    catch(System::Exception^ e)
    {
        Handle<Value> str =  v8sharp::V8Interop::ToV8(e->Message);
        Handle<v8::String> err = Handle<v8::String>::Cast(str);

        return ThrowException(v8::Exception::Error(err));
    }
}

Handle<Value> WrapBase::New()
{
    HandleScope scope;

    Local<ObjectTemplate> oTemplate = ObjectTemplate::New();
    oTemplate->SetInternalFieldCount(1);
    Local<v8::Object> result = oTemplate->NewInstance();

    WrapBase* self = new WrapBase();
    self->Wrap(result);

    result->Set(v8::String::NewSymbol("list"), FunctionTemplate::New(ListAssemblies)->GetFunction() );
    result->Set(v8::String::NewSymbol("load"), FunctionTemplate::New(LoadAssembly)->GetFunction() );

    return scope.Close(result);
}