#pragma once
#include <vector>
#include <string>
#include <typeindex>

#include "Transferrer.h"
#include "LightReflection/Runtime/Type.h"

namespace Light
{
    enum class DataType:uint8_t
    {
        Field, Array, Class
    };

    class DataTransferrer;
    template <typename TValue>
    struct DataTransferrer_Transfer
    {
        static void Invoke(DataTransferrer& serializer, TValue& value);
    };

    class DataTransferrer
    {
    public:
        virtual ~DataTransferrer() = default;

        template <class TValue>
        void TransferField(const char* name, TValue& value)
        {
            PushNode(name, DataType::Field);
            Transfer(value);
            PopNode();
        }
        /**
         * 基础类型
         * @tparam TValue 
         * @param name 
         * @param value 
         */
        template <class TValue> requires
            std::is_arithmetic_v<TValue>
            || std::is_same_v<TValue, std::string>
            || std::is_same_v<TValue, std::vector<std::byte>>
        void TransferField(const char* name, TValue& value)
        {
            PushNode(name, DataType::Field);
            Transfer(value);
            PopNode();
        }
        /**
         * 容器类型
         * @tparam TValue 
         * @param name 
         * @param value 
         */
        template <class TValue> requires
            requires(TValue container) { std::size(container);std::begin(container);std::end(container); }
            && !std::is_same_v<TValue, std::vector<std::byte>>
            && !std::is_same_v<TValue, std::string>
        void TransferField(const char* name, TValue& value)
        {
            PushNode(name, DataType::Class);
            {
                size_t size = std::size(value);
                TransferField("size", size);
                if constexpr (requires() { value.resize(size); })
                    value.resize(size);
            }
            {
                PushNode("data", DataType::Array);
                for (auto it = std::begin(value); it != std::end(value); ++it)
                    Transfer(*it);
                PopNode();
            }
            PopNode();
        }
        /**
         * std::vector<bool>特殊处理
         * @param name 
         * @param value 
         */
        void TransferField(const char* name, std::vector<bool>& value);

        virtual void PushNode(const char* name, DataType dataType) =0;
        virtual void PopNode() =0;
        virtual void Transfer(void* value, std::type_index typeIndex);
        virtual void Transfer(double& value) = 0;
        virtual void Transfer(float& value);
        virtual void Transfer(int64_t& value) = 0;
        virtual void Transfer(int32_t& value);
        virtual void Transfer(uint64_t& value);
        virtual void Transfer(uint32_t& value);
        virtual void Transfer(bool& value);
        virtual void Transfer(char& value);
        virtual void Transfer(std::string& value) = 0;
        virtual void Transfer(std::vector<std::byte>& value) = 0;
        template <class TValue>
        void Transfer(TValue& value)
        {
            DataTransferrer_Transfer<TValue>::Invoke(*this, value);
        }
    };
    static_assert(Transferrer<DataTransferrer>);

    template <typename TValue>
    void DataTransferrer_Transfer<TValue>::Invoke(DataTransferrer& serializer, TValue& value)
    {
        Type* type = Type::GetType(typeid(TValue));
        if (type != nullptr)
            type->serialize(serializer, &value);
        else
            serializer.Transfer(&value, typeid(TValue));
    }
}