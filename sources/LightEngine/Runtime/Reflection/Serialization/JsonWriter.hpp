#pragma once

#include <stack>
#include <rapidjson/document.h>

#include "JsonSerializer.hpp"
#include "Serializer.hpp"
#include "LightEngine/Runtime/Utility/String.h"

namespace Light
{
    class JsonWriter : public JsonSerializer
    {
    public:
        JsonWriter(rapidjson::Document& document): JsonSerializer(document)
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

            rapidjson::Value& parentNode = *nodes.top(); //获取父节点
            rapidjson::GenericValue<rapidjson::UTF8<>> keyString(
                name, static_cast<rapidjson::SizeType>(strlen(name)), allocator); //创建节点名
            parentNode.AddMember(keyString, 0, allocator); //创建节点

            rapidjson::Value& currentNode = parentNode[name];
            if (nodeType == NodeType::Array)
                currentNode.SetArray();
            else if (nodeType == NodeType::Class)
                currentNode.SetObject();
            nodes.push(&currentNode);
        }

        template <class TValue>
        void Transfer(const TValue& value)
        {
            if (nodeTypes.top() == NodeType::Array)
                nodes.top()->PushBack(value, allocator);
            else
                nodes.top()->Set<TValue>(value);
        }
        void Transfer(const std::string& value)
        {
            rapidjson::GenericValue genericValue = CreateString(value);
            if (nodeTypes.top() == NodeType::Array)
                nodes.top()->PushBack(genericValue, allocator);
            else
                nodes.top()->SetString(value.data(), static_cast<rapidjson::SizeType>(value.size()), allocator);
        }
        void Transfer(const std::vector<std::byte>& value)
        {
            String::EncodingBase64(value, buffer);
            Transfer(buffer);
        }

    private:
        std::string buffer = {};
    };
}