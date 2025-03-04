#pragma once
#include <stack>
#include <rapidjson/document.h>
#include "JsonSerializer.h"
#include "GleamUtility/Runtime/String.h"

namespace Gleam
{
    class JsonReader : public JsonSerializer
    {
    public:
        JsonReader(rapidjson::Document& document): JsonSerializer(document)
        {
        }

        void PushNode(const std::optional<std::string_view> name, const DataType dataType) override
        {
            DataType wrapDataType = dataTypes.top();
            int& wrapItemIndex = itemIndices.top();
            rapidjson::Value& wrap = *nodes.top();

            rapidjson::Value* currentNode;

            if (!name.has_value() && wrapDataType == DataType::Field) //重设旧节点类型
            {
                currentNode = nodes.top();
            }
            else //进入新节点
            {
                wrapItemIndex++; //当前获取的成员在容器中的索引
                if (wrapDataType == DataType::Class) //在类中基于名称获取成员
                {
                    currentNode = &wrap.operator[](name->data());
                }
                else if (wrapDataType == DataType::Array) //在数组中基于索引创建成员
                {
                    currentNode = &wrap.GetArray()[wrapItemIndex];
                }
                else
                {
                    throw std::runtime_error("在非容器结构中获取成员！");
                }
            }

            PushStruct(currentNode, dataType);
        }
        void PopNode() override
        {
            PopStruct();
        }

        template <class TValue>
        void TransferT(TValue& value)
        {
            if (dataTypes.top() == DataType::Array)
            {
                itemIndices.top()++;
                value = nodes.top()->GetArray()[itemIndices.top()].Get<TValue>();
            }
            else
                value = nodes.top()->Get<TValue>();
        }

        void Transfer(double& value) override { TransferT(value); }
        void Transfer(int64_t& value) override { TransferT(value); }
        void Transfer(std::string& value) override
        {
            if (dataTypes.top() == DataType::Array)
            {
                itemIndices.top()++;
                value = nodes.top()->GetArray()[itemIndices.top()].GetString();
            }
            else
                value = nodes.top()->GetString();
        }
        void Transfer(std::vector<std::byte>& value) override
        {
            Transfer(buffer);
            String::DecodingBase64(buffer, value);
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