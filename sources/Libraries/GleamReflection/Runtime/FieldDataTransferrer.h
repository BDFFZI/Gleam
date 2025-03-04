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

    class FieldDataTransferrer;
    /**
     * 自定义特定类型的字段传输器
     *
     * 由于基于模板，因此针对各种类型扩展都非常方便，但也因此仅对直接调用有效，使用RTTI访问将绕过该函数。
     * 建议仅用于扩展基于模板的基础类型，例如向量，其通过模板实现，同时一般都作为类成员使用而不是独立存储。
     * 其他类型一般情况下利用Type注册序列化函数即可，这样利用RTTI访问也可以成功序列化。
     */
    template <typename TValue>
    struct FieldDataTransferrer_Transfer
    {
        static void Invoke(FieldDataTransferrer& serializer, TValue& value);
    };

    /**
     * 字段数据传输器，用于实现对象的序列化。
     * 
     * 该传输器能在没有任何信息损耗的情况传递成员数据。相比Type，可以充分发挥模板和无损类型信息的优势。
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

        virtual void PushNode(std::optional<std::string_view> name, DataType dataType) =0;
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
        virtual void Transfer(uuids::uuid& value);
        template <class TValue>
        void Transfer(TValue& value)
        {
            //对于无法直接处理的字段数据类型，首先尝试使用其自定义的数据传输器
            FieldDataTransferrer_Transfer<TValue>::Invoke(*this, value);
        }
    };
    static_assert(FieldTransferrer<FieldDataTransferrer>);

    //若传递的数据类型没有实现传输函数，默认直接改用RTTI传递给虚函数，以便转交给传输器自身处理。
    template <typename TValue>
    void FieldDataTransferrer_Transfer<TValue>::Invoke(FieldDataTransferrer& serializer, TValue& value)
    {
        serializer.Transfer(&value, typeid(TValue));
    }
}