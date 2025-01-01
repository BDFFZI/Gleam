#pragma once
#include <stack>
#include <typeindex>
#include <rapidjson/document.h>

#include "Serializer.hpp"

namespace Light
{
    class JsonReader : public Serializer
    {
    public:
        JsonReader(rapidjson::Document& document)
            : allocator(document.GetAllocator()), nodePath({&document})
        {
        }
        JsonReader(JsonReader&) = delete;

        void Transfer(void* value, std::type_index type) override;
        void PushPath(const char* name) override;
        void PopPath() override;

    private:
        rapidjson::Document::AllocatorType& allocator;
        std::stack<rapidjson::Value*> nodePath;
    };
}