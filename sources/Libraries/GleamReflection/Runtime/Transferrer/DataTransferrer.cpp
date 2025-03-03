#include "DataTransferrer.h"

#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    void DataTransferrer::TransferField(const char* name, std::vector<bool>& value)
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
    void DataTransferrer::Transfer(void* value, const std::type_index typeIndex)
    {
        auto optionalType = Type::GetType(typeIndex);
        if (optionalType.has_value())
        {
            Type& type = optionalType.value();
            PushNode(nullptr, DataType::Class);
            type.GetSerialize()(*this, value);
            PopNode();
        }
        else
            throw std::runtime_error("不支持的传输类型！");
    }
    void DataTransferrer::Transfer(float& value)
    {
        double wrap = value;
        Transfer(wrap);
        value = static_cast<float>(wrap);
    }
    void DataTransferrer::Transfer(int32_t& value)
    {
        int64_t wrap = value;
        Transfer(wrap);
        value = static_cast<int32_t>(wrap);
    }
    void DataTransferrer::Transfer(uint64_t& value)
    {
        int64_t wrap = static_cast<int64_t>(value);
        Transfer(wrap);
        value = static_cast<uint64_t>(wrap);
    }
    void DataTransferrer::Transfer(uint32_t& value)
    {
        int32_t wrap = static_cast<int32_t>(value);
        Transfer(wrap);
        value = static_cast<uint32_t>(wrap);
    }
    void DataTransferrer::Transfer(bool& value)
    {
        int64_t wrap = value;
        Transfer(wrap);
        value = static_cast<bool>(wrap);
    }
    void DataTransferrer::Transfer(char& value)
    {
        int64_t wrap = value; // NOLINT(bugprone-signed-char-misuse)
        Transfer(wrap);
        value = static_cast<char>(wrap);
    }
    void DataTransferrer::Transfer(uuids::uuid& value)
    {
        std::string wrap = to_string(value);
        Transfer(wrap);
        value = uuids::uuid::from_string(wrap).value_or(uuids::uuid{});
    }
}