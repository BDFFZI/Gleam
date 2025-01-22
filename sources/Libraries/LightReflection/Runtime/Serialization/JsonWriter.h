#pragma once

#include <stack>
#include <rapidjson/document.h>
#include "JsonSerializer.h"
#include "LightUtility/Runtime/String.h"

namespace Light
{
    class JsonWriter : public JsonSerializer
    {
    public:
        JsonWriter(rapidjson::Document& document): JsonSerializer(document)
        {
        }

        void PushNode(const char* name, const DataType dataType) override
        {
            DataType wrapDataType = dataTypes.top();
            int& wrapItemIndex = itemIndices.top();
            rapidjson::Value& wrap = *nodes.top();

            rapidjson::Value* currentNode;
            if (name == nullptr && wrapDataType == DataType::Field) //重设旧节点类型
            {
                currentNode = nodes.top();
            }
            else //创建新节点 
            {
                wrapItemIndex++; //新节点在容器中的索引
                if (wrapDataType == DataType::Class) //在类中基于名称创建成员
                {
                    rapidjson::GenericValue key = CreateString(name);
                    wrap.AddMember(key, 0, *allocator);
                    currentNode = &wrap[name];
                }
                else if (wrapDataType == DataType::Array) //在数组中基于索引创建成员
                {
                    wrap.PushBack(0, *allocator);
                    currentNode = &wrap.GetArray()[wrapItemIndex];
                }
                else
                {
                    throw std::runtime_error("在非容器结构中创建成员！");
                }
            }

            if (dataType == DataType::Array)
                currentNode->SetArray();
            else if (dataType == DataType::Class)
                currentNode->SetObject();

            PushStruct(currentNode, dataType);
        }
        void PopNode() override
        {
            PopStruct();
        }

        template <class TValue>
        void TransferT(const TValue& value)
        {
            if (dataTypes.top() == DataType::Array)
            {
                itemIndices.top()++;
                nodes.top()->PushBack(value, *allocator);
            }
            else
                nodes.top()->Set<TValue>(value);
        }

        void Transfer(double& value) override { TransferT(value); }
        void Transfer(int64_t& value) override { TransferT(value); }
        void Transfer(std::string& value) override
        {
            rapidjson::GenericValue genericValue = CreateString(value);
            if (dataTypes.top() == DataType::Array)
            {
                itemIndices.top()++;
                nodes.top()->PushBack(genericValue, *allocator);
            }
            else
                nodes.top()->SetString(value.data(), static_cast<rapidjson::SizeType>(value.size()), *allocator);
        }
        void Transfer(std::vector<std::byte>& value) override
        {
            String::EncodingBase64(value, buffer);
            Transfer(buffer);
        }

        void Transfer(float& value) override { TransferT(value); }
        void Transfer(int32_t& value) override { TransferT(value); }
        void Transfer(uint64_t& value) override { TransferT(value); }
        void Transfer(uint32_t& value) override { TransferT(value); }
        void Transfer(bool& value) override { TransferT(value); }

    private:
        std::string buffer = {};
    };
}