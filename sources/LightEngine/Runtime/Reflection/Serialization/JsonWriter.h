#pragma once

#include <stack>
#include <rapidjson/document.h>

#include "Serializer.hpp"

namespace Light
{
    class JsonWriter : public Serializer
    {
    public:
        JsonWriter(rapidjson::Document& document)
            : allocator(document.GetAllocator()), nodePath({&document})
        {
        }
        JsonWriter(JsonWriter&) = delete;

        void Transfer(void* value, std::type_index type) override;
        void PushPath(const char* name) override;
        void PopPath() override;

    private:
        rapidjson::Document::AllocatorType& allocator;
        std::stack<rapidjson::Value*> nodePath;
    };
}