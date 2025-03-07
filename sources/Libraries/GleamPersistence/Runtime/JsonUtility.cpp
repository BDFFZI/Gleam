﻿#include "JsonUtility.h"

#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "Serializer/JsonReader.h"
#include "Serializer/JsonWriter.h"

namespace Gleam
{
    rapidjson::Document JsonUtility::ToDocument(void* object,const Type& type)
    {
        rapidjson::Document document;
        document.Parse("{}");
        JsonWriter jsonWriter = {document};
        type.Serialize(jsonWriter, object);
        return document;
    }
    void JsonUtility::FromDocument(rapidjson::Document& doc,const Type& type, void* address)
    {
        type.Construct(address);
        JsonReader jsonReader = {doc};
        type.Serialize(jsonReader, address);
    }
    std::string JsonUtility::ToJson(void* object, const Type& type, const bool pretty)
    {
        return DocumentToJson(ToDocument(object, type), pretty);
    }
    void JsonUtility::FromJson(std::string& json,const Type& type, void* address)
    {
    }
    std::string JsonUtility::DocumentToJson(const rapidjson::Document& document, const bool pretty)
    {
        rapidjson::StringBuffer buffer;
        if (pretty == false)
        {
            rapidjson::Writer writer(buffer);
            document.Accept(writer);
        }
        else
        {
            rapidjson::PrettyWriter writer(buffer);
            document.Accept(writer);
        }
        return buffer.GetString();
    }
}