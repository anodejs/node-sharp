#pragma comment(lib, "node")

#include <node.h>
#include <v8.h>
#include <string>

#include "SharpLibHelper.h"

using namespace node;
using namespace v8;

class SharpObject: ObjectWrap
{
private:
    SharpLibHelper* _sharpLibHelper;
public:

    static Persistent<FunctionTemplate> s_ct;
    static void NODE_EXTERN Init(Handle<Object> target)
    {
        HandleScope scope;
        
        target->Set(v8::String::NewSymbol("Sharp"), FunctionTemplate::New(SharpLibHelper::Instance)->GetFunction());
    }
};


Persistent<FunctionTemplate> SharpObject::s_ct;

extern "C" {
    void NODE_EXTERN init (Handle<Object> target)
    {
        SharpObject::Init(target);

        // Load Assembly is a global method defined in SharpLibHelper.
        // It attaches a hander to AssemblyLoad to search the active directory.
        LoadAssembly();
    }
    // 'sharp' here is treated as a string within the macro
    NODE_MODULE(sharp, init);
}