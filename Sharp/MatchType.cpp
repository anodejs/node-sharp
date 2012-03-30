#include "MatchType.h"

using namespace System;
using namespace System::Reflection;
using namespace System::Collections::Generic;


Object^ CreateObject(Dictionary<String^, Object^>^ dictionary, Type^ type)
{
    ConstructorInfo^ constructor = type->GetConstructor(gcnew array<Type^>{});

    if (constructor == nullptr)
    {
        throw gcnew Exception("Constructing objects requires a zero arg constructor.");
    }

    Object^ o = constructor->Invoke(gcnew array<Object^>{});

    for each(String^ key in dictionary->Keys)
    {
        Object^ value = dictionary[key];

        // find a field
        FieldInfo^ f = type->GetField(key);
        PropertyInfo^ p = type->GetProperty(key);

        if (f != nullptr)
        {
            Object^ adjusted = MatchType::AdjustObject(value, f->FieldType);
            f->SetValue(o, adjusted);
        }
        else if (p != nullptr)
        {
            p->SetValue(o, MatchType::AdjustObject(value, p->PropertyType), nullptr);
        }
        else
        {
            throw gcnew Exception(String::Format("No matching field found for {0} on {1}", key, type->Name));
        }
    }

    return o;
}

Object^ CreateArray(Array^ input, Type^ t)
{
    Type^ arrayType = t->GetElementType();
    ConstructorInfo^ c = t->GetConstructor(gcnew array<Type^> {int::typeid});

    if (c == nullptr)
    {
        throw gcnew Exception(String::Format("Failed to find constructor for array type {0}", t));
    }

    Array^ result = (Array^)c->Invoke(gcnew array<Object^> {input->Length});

    int index = 0;
    for each(Object^ item in input)
    {
        Object^ converted = MatchType::AdjustObject(item, arrayType);
        result->SetValue(converted, index++);
    }

    return result;
}

Object^ CreateList(Array^ input, Type^ t)
{
    Type^ listType = t->GetGenericArguments()[0];
    ConstructorInfo^ c = t->GetConstructor(gcnew array<Type^>{int::typeid});

    if (c == nullptr)
    {
        throw gcnew Exception(String::Format("Failed to find constructor for array type {0}", t));
    }

    Object^ result = c->Invoke(gcnew array<Object^> {input->Length});

    MethodInfo^ addMethod = t->GetMethod("Add");

    for each (Object^ item in input)
    {
        Object^ converted = MatchType::AdjustObject(item, listType);
        addMethod->Invoke(result, gcnew array<Object^>{converted});
    }

    return result;
}

array<Object^>^ MatchType::AdjustArguments(array<Object^>^ paramList, array<ParameterInfo^>^ paramTypes)
{
    List<Object^>^ result = gcnew List<Object^>();
    for (int i = 0; i < paramTypes->Length; i++ )
    {
        Object^ obj = (paramList->Length > i) ? paramList[i] : nullptr;
        Type^ paramType = paramTypes[i]->ParameterType;
        Object^ adjusted = MatchType::AdjustObject(obj, paramType);

        result->Add(adjusted);
    }

    return result->ToArray();
}

Object^ MatchType::AdjustObject(Object^ obj, Type^ paramType)
{
    static gcroot<Type^> DictionaryType = Dictionary<String^, Object^>::typeid;
    static gcroot<Type^> GenericList = List<int>::typeid->GetGenericTypeDefinition();
    if (obj == nullptr){ return nullptr; }

    Type^ t = obj->GetType();

    if (paramType->IsAssignableFrom(t))
    {
        // aready good.
        return obj;
    }

    // case 1 - generated a dictionary.
    if (t->IsAssignableFrom(DictionaryType))
    {
        return CreateObject(safe_cast<Dictionary<String^, Object^>^>(obj), paramType);
    }

    // case 2 - generated an array
    if (t->IsArray)
    {
        if (paramType->IsArray)
        {
            return CreateArray(safe_cast<Array^>(obj), paramType);
        }

        if (paramType->IsGenericType && paramType->GetGenericTypeDefinition()->IsAssignableFrom(GenericList))
        {
            // assigning to a list.
            return CreateList(safe_cast<Array^>(obj), paramType);
        }

        // some more complicated vector? give up
        throw gcnew Exception(String::Format("Could not convert array types {0} to {1}", t, paramType));
    }

    // couldn't figure out a good conversion, give up.
    return obj;
}

