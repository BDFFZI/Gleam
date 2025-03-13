#pragma once
#include <vector>
#include <string>
#include <typeindex>
#include <stduuid/uuid.h>

#include "FieldTransferrer.h"

namespace Gleam
{
    enum class DataType:uint8_t
    {
        Field, Array, Class
    };

    /**
     * 自定义针对特殊类型的字段传输器（不直接使用函数模板，是因为函数模板不支持部分特化，但若无需部分特化，建议直接特化函数来减少堆栈）
     *
     * 由于基于模板，因此针对各种类型扩展都非常方便，但也因此仅对直接调用有效，使用RTTI访问将绕过该函数。
     * 建议仅用于扩展基于模板的基础类型，例如向量，其通过模板实现，同时一般都作为类成员使用而不是独立存储。
     * 其他类型一般情况下利用Type注册序列化函数即可，这样利用RTTI访问也可以成功序列化。
     */
    template <typename TValue>
    struct FieldDataTransferrer_Transfer
    {
        // 若要实现自定义传输，请实现下方注释的函数
        // static void Invoke(FieldDataTransferrer& serializer, TValue& value);
    };

    /**
     * 字段数据传输器，以此实现在没有任何信息损耗的情况完整递归的遍历所有成员数据。相比Type访问成员数据，该传输器可以充分发挥模板和无损类型信息的优势。
     */
    class FieldDataTransferrer
    {
    public:
        virtual ~FieldDataTransferrer() = default;

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
        void TransferField(std::string_view name, TValue& value)
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
            (requires(TValue container) { std::size(container);std::begin(container);std::end(container); }
                && !std::is_same_v<TValue, std::vector<std::byte>>
                && !std::is_same_v<TValue, std::string>)
            || std::is_array_v<TValue>
        void TransferField(std::string_view name, TValue& value)
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
        void TransferField(std::string_view name, std::vector<bool>& value);
        /**
         * 其他类型
         * @tparam TValue 
         * @param name 
         * @param value 
         */
        template <class TValue>
        void TransferField(std::string_view name, TValue& value)
        {
            PushNode(name, DataType::Field);
            Transfer(value);
            PopNode();
        }

        virtual void PushNode(std::optional<std::string_view> name, DataType dataType)
        {
        }
        virtual void PopNode()
        {
        }
        virtual void Transfer(void* value, std::type_index typeIndex);
        virtual void Transfer(double& value)
        {
        }
        virtual void Transfer(float& value);
        virtual void Transfer(int64_t& value)
        {
        }
        virtual void Transfer(int32_t& value);
        virtual void Transfer(uint64_t& value);
        virtual void Transfer(uint32_t& value);
        virtual void Transfer(bool& value);
        virtual void Transfer(char& value);
        virtual void Transfer(std::string& value)
        {
        }
        virtual void Transfer(std::vector<std::byte>& value)
        {
        }
        virtual void Transfer(uuids::uuid& value);
        template <class TValue>
        void Transfer(TValue& value) //传输特殊字段
        {
            if constexpr (requires() { FieldDataTransferrer_Transfer<TValue>::Invoke; })
                FieldDataTransferrer_Transfer<TValue>::Invoke(*this, value); //优先使用自定义实现
            else
                this->Transfer(&value, typeid(value)); //否则用RTTI处理
        }
    };
    static_assert(FieldTransferrer<FieldDataTransferrer>);
}