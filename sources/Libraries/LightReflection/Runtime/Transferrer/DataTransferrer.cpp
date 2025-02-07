#include "DataTransferrer.h"

#include "LightReflection/Runtime/Type.h"

namespace Light
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
        Type* type = Type::GetType(typeIndex);
        if (type != nullptr)
        {
            PushNode(nullptr, DataType::Class);
            type->serialize(*this, value);
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
}