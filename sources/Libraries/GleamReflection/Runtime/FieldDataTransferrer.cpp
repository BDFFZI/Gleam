#include "FieldDataTransferrer.h"

#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    void FieldDataTransferrer::TransferField(std::string_view name, std::vector<bool>& value)
    {
        PushNode(name, DataType::Class);
        {
            size_t size = std::size(value);
            TransferField("size", size);
            value.resize(size);
        }
        {
            PushNode("data", DataType::Array);
            for (auto it : value)
            {
                bool element = it;
                Transfer(element);
                it = element;
            }
            PopNode();
        }
        PopNode();
    }
    void FieldDataTransferrer::Transfer(void* value, const std::type_index typeIndex)
    {
        auto optionalType = Type::GetType(typeIndex);
        if (optionalType.has_value())
        {
            const Type& type = optionalType.value();
            PushNode(std::nullopt, DataType::Class);
            type.Serialize(*this, value);
            PopNode();
        }
        else
            throw std::runtime_error("不支持的传输类型！");
    }
    void FieldDataTransferrer::Transfer(float& value)
    {
        double wrap = value;
        Transfer(wrap);
        value = static_cast<float>(wrap);
    }
    void FieldDataTransferrer::Transfer(int32_t& value)
    {
        int64_t wrap = value;
        Transfer(wrap);
        value = static_cast<int32_t>(wrap);
    }
    void FieldDataTransferrer::Transfer(uint64_t& value)
    {
        int64_t wrap = static_cast<int64_t>(value);
        Transfer(wrap);
        value = static_cast<uint64_t>(wrap);
    }
    void FieldDataTransferrer::Transfer(uint32_t& value)
    {
        int32_t wrap = static_cast<int32_t>(value);
        Transfer(wrap);
        value = static_cast<uint32_t>(wrap);
    }
    void FieldDataTransferrer::Transfer(bool& value)
    {
        int64_t wrap = value;
        Transfer(wrap);
        value = static_cast<bool>(wrap);
    }
    void FieldDataTransferrer::Transfer(char& value)
    {
        int64_t wrap = value; // NOLINT(bugprone-signed-char-misuse)
        Transfer(wrap);
        value = static_cast<char>(wrap);
    }
    void FieldDataTransferrer::Transfer(uuids::uuid& value)
    {
        std::string wrap = to_string(value);
        Transfer(wrap);
        value = uuids::uuid::from_string(wrap).value_or(uuids::uuid{});
    }
}