#include "WrapInstance.h"
#include "v8value.h"
#include "MatchType.h"
#include "Helpers.h"

using namespace System::Collections::Generic;
using namespace System::Reflection;
using namespace v8;

Handle<Value> WrapInstance::New(System::Type^ t, System::Object^ o)
{   
    HandleScope scope;

    Local<ObjectTemplate> oTemplate = ObjectTemplate::New();
    oTemplate->SetInternalFieldCount(1);
    Local<v8::Object> result = oTemplate->NewInstance();

    WrapInstance* self = new WrapInstance(t,o);
    self->Wrap(result);

    result->Set(v8::String::NewSymbol("class"), v8sharp::V8Interop::ToV8(self->_type->Name));
    result->Set(v8::String::NewSymbol("methods"), FunctionTemplate::New(ListMethods)->GetFunction() );
    result->Set(v8::String::NewSymbol("call"), FunctionTemplate::New(CallMethod)->GetFunction() );
    result->Set(v8::String::NewSymbol("async"), FunctionTemplate::New(Async)->GetFunction() );
        
    result->SetHiddenValue(v8::String::NewSymbol("__wrapper"), v8::Boolean::New(true));

    // Debug code for checking how the hidden values might work.
    //Handle<v8::Value> val1 = result->GetHiddenValue(v8::String::NewSymbol("__wrapper"));
    //Handle<v8::Value> val2 = result->GetHiddenValue(v8::String::NewSymbol("__bogus"));

    //bool null1 = val1.IsEmpty(); 
    //bool null2 = val2.IsEmpty();

    return scope.Close(result);
}

bool WrapInstance::IsWrapped(v8::Handle<v8::Value> value)
{
    v8::Handle<v8::Object> obj = value->ToObject();

    if (!obj.IsEmpty())
    {
        v8::Local<v8::Value> sym = obj->GetHiddenValue(v8::String::NewSymbol("__wrapper"));
        if (!sym.IsEmpty() && sym->IsBoolean())
        {
            return sym->IsTrue();
        }
    }

    return false;
}

System::Object^ WrapInstance::Unwrap(v8::Handle<v8::Value> value)
{
    HandleScope scope;
    v8::Handle<v8::Object> obj = value->ToObject();
    WrapInstance* self = node::ObjectWrap::Unwrap<WrapInstance>(obj);
	return self->_instance;
}

WrapInstance::WrapInstance(System::Type^ t, System::Object^ i)
{
    _type = t;
    _instance = i;
}

Handle<Value> WrapInstance::GetClassName(const Arguments& args)
{
    HandleScope scope;
    WrapInstance* self = node::ObjectWrap::Unwrap<WrapInstance>(args.This());
        
    Handle<Value> result = v8sharp::V8Interop::ToV8(self->_type->Name);

    return scope.Close(result);
}
    
Handle<Value> WrapInstance::ListMethods(const Arguments& args)
{
    HandleScope scope;
    WrapInstance* self = node::ObjectWrap::Unwrap<WrapInstance>(args.This());
        
    List<System::String^>^ methods = gcnew List<System::String^>(); 
        
    for each(MethodInfo^ m in self->_type->GetMethods())
    {
        methods->Add(m->Name);
    }
        
    Handle<Value> result = v8sharp::V8Interop::ToV8(methods->ToArray());

    return scope.Close(result);
}

Handle<Value> WrapInstance::CallMethod(const Arguments& args)
{
    if (args.Length() < 1 || !args[0]->IsString()) {
        return ThrowException(v8::Exception::TypeError(
            v8::String::New("First argument must be a string, for method name.")));
    }

    System::String^ path =  safe_cast<System::String^>(v8sharp::V8Interop::FromV8(args[0]));

    HandleScope scope;
    WrapInstance* self = node::ObjectWrap::Unwrap<WrapInstance>(args.This());

    try
    {
        MethodInfo^ m = self->_type->GetMethod(path);

        if (m == nullptr)
        {
            throw gcnew System::NotSupportedException(System::String::Format("Method {0} not found", path));
        }
        
        array<System::Object^>^ argList = Helpers::ConvertArguments(args, 1, m->GetParameters());

        System::Object^ r = m->Invoke(self->_instance, argList);
                    
        Handle<Value> result = v8sharp::V8Interop::ToV8(r);
        
        return scope.Close(result);
    }
    catch(System::Exception^ e)
    {
        v8::Handle<v8::Value> str =  v8sharp::V8Interop::ToV8(Helpers::GetError(e));
        v8::Handle<v8::String> err = v8::Handle<v8::String>::Cast(str);

        return ThrowException(v8::Exception::Error(err));
    }
}
    
Handle<Value> WrapInstance::Async(const Arguments& args)
{
    HandleScope scope;

    if (!args[0]->IsString()) {
        return ThrowException(v8::Exception::TypeError(
            v8::String::New("First argument must be a string, for method name")));
    }

    if (!args[1]->IsFunction()) {
        return ThrowException(v8::Exception::TypeError(
            v8::String::New("Second argument must be a callback function")));
    }

    Local<v8::String> path = Local<v8::String>::Cast(args[0]);
    // There's no ToFunction(), use a Cast instead.
    Local<Function> callback = Local<Function>::Cast(args[1]);

    WrapInstance* self = node::ObjectWrap::Unwrap<WrapInstance>(args.This());

    // create a state object
    Baton* baton = new Baton();
    baton->request.data = baton;
    baton->caller = self;
    baton->callback = Persistent<Function>::New(callback);

    try
    {
        MethodInfo^ m = self->_type->GetMethod(safe_cast<System::String^>(v8sharp::V8Interop::FromV8(path)));
        baton->method = m;

        array<System::Object^>^ argList = Helpers::ConvertArguments(args, 2, m->GetParameters());
        baton->args = argList;
    }
    catch(System::Exception^ e)
    {
        v8::Handle<v8::Value> str =  v8sharp::V8Interop::ToV8(Helpers::GetError(e));
        v8::Handle<v8::String> err = v8::Handle<v8::String>::Cast(str);
            
        return ThrowException(v8::Exception::Error(err));
    }

    // register a worker thread request
    uv_queue_work(uv_default_loop(), &baton->request,
        StartAsync, AfterAsync);

    return Undefined();
}
    
// this runs on the worker thread and should not callback or interact with node/v8 in any way
void WrapInstance::StartAsync(uv_work_t* req)
{
    Baton *baton = static_cast<Baton*>(req->data);

    try
    {
        baton->error = false;
        baton->result = baton->method->Invoke(baton->caller->_instance, baton->args); 
    }
    catch(System::Exception^ e)
    {
        baton->error = true;
        baton->error_message = Helpers::GetError(e);
    }
}

// this runs on the main thread and can call back into the JavaScript
void WrapInstance::AfterAsync(uv_work_t *req)
{
    HandleScope scope;
    Baton* baton = static_cast<Baton*>(req->data);

    if (baton->error) 
    {
        Handle<v8::Value> str =  v8sharp::V8Interop::ToV8(baton->error_message);
        Handle<v8::String> err = Handle<v8::String>::Cast(str);

        Handle<Value> argv[] = { err };

        TryCatch try_catch;
        baton->callback->Call(
            Context::GetCurrent()->Global(), 1, argv);

        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }        
    } 
    else 
    {
        const unsigned argc = 2;
        Local<Value> argv[argc] = {
            Local<Value>::New(Null()),
            Local<Value>::New(v8sharp::V8Interop::ToV8(baton->result))
        };

        TryCatch try_catch;
        baton->callback->Call(Context::GetCurrent()->Global(), argc, argv);

        if (try_catch.HasCaught()) {
            node::FatalException(try_catch);
        }
    }

    baton->callback.Dispose();
    delete baton;
}
