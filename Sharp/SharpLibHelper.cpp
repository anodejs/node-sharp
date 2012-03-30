#include "SharpLibHelper.h"
#include "WrapBase.h"

#using <mscorlib.dll>

using namespace System::Reflection;
using namespace System::IO;
using namespace System;

using namespace v8;

// we need to look for the assembly in the current directory
// by default node will search next to the node.exe binary
System::Reflection::Assembly ^OnAssemblyResolve(System::Object ^obj, System::ResolveEventArgs ^args)
{
    System::String ^path = System::Environment::CurrentDirectory;

    Console::WriteLine("Resolving an assembly..." + path);

    array<System::String^>^ assemblies =
        System::IO::Directory::GetFiles(path, "*.dll");
    for (long ii = 0; ii < assemblies->Length; ii++) {
        AssemblyName ^name = AssemblyName::GetAssemblyName(assemblies[ii]);
        if (AssemblyName::ReferenceMatchesDefinition(gcnew AssemblyName(args->Name), name)) {
            return Assembly::Load(name);
        }
    }
    return nullptr;
}

// register a custom assembly load handler
void LoadAssembly()
{
    System::AppDomain::CurrentDomain->AssemblyResolve +=
        gcnew System::ResolveEventHandler(OnAssemblyResolve);
}

Handle<Value> SharpLibHelper::Instance(const v8::Arguments& args)
{
    return WrapBase::New();
}
