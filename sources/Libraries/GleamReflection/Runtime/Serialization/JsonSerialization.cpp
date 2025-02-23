#include "JsonSerialization.h"

#include <rapidjson/prettywriter.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "JsonReader.h"
#include "JsonWriter.h"

namespace Gleam
{
    rapidjson::Document Json::ToDocument(void* object, Type& type)
    {
        rapidjson::Document document;
        document.Parse("{}");
        JsonWriter jsonWriter = {document};
        type.Serialize()(jsonWriter, object);
        return document;
    }
    void Json::FromDocument(rapidjson::Document& doc, Type& type, void* address)
    {
        type.Construct()(address);
        JsonReader jsonReader = {doc};
        type.Serialize()(jsonReader, address);
    }
    std::string Json::ToJson(void* object, Type& type, const bool pretty)
    {
        return DocumentToJson(ToDocument(object, type), pretty);
    }
    void Json::FromJson(std::string& json, Type& type, void* address)
    {
        
    }
    std::string Json::DocumentToJson(const rapidjson::Document& document, const bool pretty)
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