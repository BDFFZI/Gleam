#pragma once
#include <stack>

namespace Gleam
{
    class JsonSerializer : public DataTransferrer
    {
    public:
        JsonSerializer(rapidjson::Document& document)
            : dataTypes({DataType::Class}),
              itemIndices({-1}),
              nodes({&document}),
              allocator(&document.GetAllocator())
        {
        }

    protected:
        std::stack<DataType> dataTypes = {};
        std::stack<int> itemIndices = {};
        std::stack<rapidjson::Value*> nodes;
        rapidjson::Document::AllocatorType* allocator;

        rapidjson::GenericValue<rapidjson::UTF8<>> CreateString(std::string_view string) const
        {
            return {
                string.data(),
                static_cast<rapidjson::SizeType>(string.size()),
                *allocator};
        }
        void PushStruct(rapidjson::Value* data, const DataType dataType)
        {
            dataTypes.push(dataType);
            itemIndices.push(-1);
            nodes.push(data);
        }
        void PopStruct()
        {
            dataTypes.pop();
            itemIndices.pop();
            nodes.pop();
        }
    };
}