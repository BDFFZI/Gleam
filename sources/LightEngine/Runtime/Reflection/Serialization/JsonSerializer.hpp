#pragma once
#include <stack>

#include "Serializer.hpp"

namespace Light
{
    class JsonSerializer : public Serializer
    {
    public:
        JsonSerializer(JsonSerializer&) = delete;

    protected:
        JsonSerializer(rapidjson::Document& document)
            : allocator(document.GetAllocator()), nodes({&document})
        {
        }

        std::stack<NodeType> nodeTypes;
        std::stack<int> itemIndices;
        std::stack<rapidjson::Value*> nodes;
        rapidjson::Document::AllocatorType& allocator;

        rapidjson::GenericValue<rapidjson::UTF8<>> CreateString(std::string_view string) const
        {
            return {
                string.data(),
                static_cast<rapidjson::SizeType>(string.size()),
                allocator};
        }

        void Transfer(void* value, std::type_index type) override
        {
            itemIndices.top()++;
        }
        void PushNode(const char* name, const NodeType nodeType) override
        {
            nodeTypes.push(nodeType);
            itemIndices.push(-1);
        }
        void PopNode() override
        {
            nodeTypes.pop();
            itemIndices.pop();
            nodes.pop();
        }
    };
}