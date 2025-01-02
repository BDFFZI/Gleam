#pragma once

#include <stack>
#include <rapidjson/document.h>

#include "Serializer.hpp"
#include "LightEngine/Runtime/Utility/String.h"

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

        template <class TValue>
        void Write(const TValue& value)
        {
            nodePath.top()->Set(value);
        }
        void Write(const std::string& value)
        {
            nodePath.top()->SetString(value.data(), static_cast<rapidjson::SizeType>(value.size()), allocator);
        }
        void Write(const char& value)
        {
            Write(static_cast<int>(value));
        }
        void Write(const std::vector<std::byte>& value)
        {
            Write(String::EncodingBase64(value));
        }

    private:
        rapidjson::Document::AllocatorType& allocator;
        std::stack<rapidjson::Value*> nodePath;
    };
}