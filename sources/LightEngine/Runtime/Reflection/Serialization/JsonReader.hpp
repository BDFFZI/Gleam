#pragma once
#include <stack>
#include <typeindex>
#include <rapidjson/document.h>

#include "JsonSerializer.hpp"
#include "Serializer.hpp"
#include "LightEngine/Runtime/Utility/String.h"

namespace Light
{
    class JsonReader : public JsonSerializer
    {
    public:
        JsonReader(rapidjson::Document& document): JsonSerializer(document)
        {
        }

        void Transfer(void* value, const std::type_index type) override
        {
            JsonSerializer::Transfer(value, type);
            Light_Transfer_Inner(Transfer)
            throw std::runtime_error("不支持的传输类型！");
        }
        void PushNode(const char* name, const NodeType nodeType) override
        {
            JsonSerializer::PushNode(name, nodeType);

            nodes.push(&nodes.top()->operator[](name));
        }

        template <class TValue>
        void Transfer(TValue& value)
        {
            if (nodeTypes.top() == NodeType::Array)
                value = nodes.top()->GetArray()[itemIndices.top()].Get<TValue>();
            else
                value = nodes.top()->Get<TValue>();
        }
        void Transfer(std::string& value)
        {
            if (nodeTypes.top() == NodeType::Array)
                value = nodes.top()->GetArray()[itemIndices.top()].GetString();
            else
                value = nodes.top()->GetString();
        }
        void Transfer(std::vector<std::byte>& value)
        {
            Transfer(buffer);
            String::DecodingBase64(buffer, value);
        }

    private:
        std::string buffer = {};
    };
}