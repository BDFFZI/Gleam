#pragma once
#include <string>
#include <rapidjson/document.h>

#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class JsonUtility
    {
    public:
        static rapidjson::Document ToDocument(void* object,const Type& type);
        static void FromDocument(rapidjson::Document& doc,const Type& type, void* address);

        static std::string ToJson(void* object,const Type& type, bool pretty = false);
        static void FromJson(std::string& json,const Type& type, void* address);

        static std::string DocumentToJson(const rapidjson::Document& document, bool pretty = false);
    };
}