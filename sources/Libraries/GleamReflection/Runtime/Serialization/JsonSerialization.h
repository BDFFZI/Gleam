#pragma once
#include <string>
#include <rapidjson/document.h>

#include "../Type.h"

namespace Gleam
{
    class Json
    {
    public:
        static rapidjson::Document ToDocument(void* object, Type* type);
        static void FromDocument(rapidjson::Document& doc, Type* type, void* address);

        static std::string ToJson(void* object, Type* type, bool pretty = false);
        static void FromJson(std::string& json, Type* type, void* address);

        static std::string DocumentToJson(const rapidjson::Document& document, bool pretty = false);
    };
}