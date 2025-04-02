#pragma once
#include <vector>
#include <string>
#include <typeindex>
#include <stduuid/uuid.h>
#include <filesystem>

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

        template <class TValue>
        void TransferField(std::string_view name, TValue& value)
        {
            PushNode(name, DataType::Field);
            Transfer(value);
            PopNode();
        }

        /**
         * 枚举类型
         * @tparam TValue 
         * @param value 
         */
        template <class TValue> requires std::is_enum_v<TValue>
        void Transfer(TValue& value)
        {
            using type = std::_Unwrap_enum_t<TValue>;
            Transfer(*reinterpret_cast<type*>(&value));
        }
        /**
         * 元组类型
         */
        template <class... TValue>
        void Transfer(std::tuple<TValue...>& value)
        {
            auto TransferTuple = [this]<size_t... Indices>(
                std::tuple<TValue...>& value, std::index_sequence<Indices...>)
            {
                PushNode(std::nullopt, DataType::Class);
                (this->TransferField(std::format("item_{}", Indices), std::get<Indices>(value)), ...);
                PopNode();
            };

            TransferTuple(value, std::make_index_sequence<sizeof...(TValue)>());
        }
        /**
         * 字典类型
         * @tparam TValue 
         * @param map 
         */
        template <class TKey, class TValue>
        void Transfer(std::unordered_map<TKey, TValue>& map)
        {
            PushNode(std::nullopt, DataType::Class);
            {
                size_t size = std::size(map);
                TransferField("size", size);

                PushNode("data", DataType::Array);
                if (size > std::size(map)) //反序列化
                {
                    for (size_t i = 0; i < size; i++)
                    {
                        TKey key;
                        TValue value;
                        PushNode(std::nullopt, DataType::Class); //在数组容器内，所有序列化器都会忽略字段名称，故序列化和反序列化时允许不一致
                        TransferField("key", key);
                        TransferField("value", value);
                        PopNode();
                        map.insert({key, value});
                    }
                }
                else //序列化
                {
                    static std::vector<typename std::unordered_map<TKey, TValue>::node_type> nodes = {};

                    //提取所有节点
                    nodes.clear();
                    while (!map.empty())
                        nodes.push_back(map.extract(map.begin()));
                    //修改并插回节点
                    for (auto& node : nodes)
                    {
                        //我们需要一个稳定的锚点，但map的项顺序是不定的。
                        //如果直接将顺序作为锚点，那逻辑上不同的项，在顺序改变后，可能在ImGui会使用相同的ID。
                        //因此改为利用map中节点值地址不变和extract移动节点的特性，获得稳定的地址值作为锚点。
                        uintptr_t address = reinterpret_cast<uintptr_t>(&node.mapped());
                        PushNode(std::to_string(address), DataType::Class);
                        TransferField("key", node.key());
                        TransferField("value", node.mapped());
                        PopNode();

                        map.insert(std::move(node));
                    }
                }
                PopNode();

                //额外增减
                if (size > std::size(map) && requires() { TKey{};TValue{}; })
                    map.insert({TKey{}, TValue{}});
                else if (size < std::size(map))
                    map.erase(TKey{});
            }
            PopNode();
        }
        /**
         * 容器类型
         * @tparam TValue 
         * @param value 
         */
        template <class TValue> requires
            (requires(TValue container) { std::size(container);std::begin(container);std::end(container); }
                && !std::is_same_v<TValue, std::vector<std::byte>>
                && !std::is_same_v<TValue, std::string>)
            || std::is_array_v<TValue>
        void Transfer(TValue& value)
        {
            PushNode(std::nullopt, DataType::Class);
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
         * 容器类型std::vector<bool>特殊处理
         * @param value 
         */
        void Transfer(std::vector<bool>& value);
        /**
         * 指针类型
         * @tparam TValue 
         * @param value 
         */
        template <class TValue>
        void Transfer(std::weak_ptr<TValue>& value)
        {
            if constexpr (requires() { FieldDataTransferrer_Transfer<std::weak_ptr<TValue>>::Invoke; })
                FieldDataTransferrer_Transfer<std::weak_ptr<TValue>>::Invoke(*this, value); //优先使用自定义实现
            else
                this->FallbackTransferPtr(*reinterpret_cast<std::weak_ptr<void>*>(&value), typeid(TValue)); //否则用专门的指针传输回退函数
        }
        /**
         * 其他任意类型
         * @tparam TValue 
         * @param value 
         */
        template <class TValue>
        void Transfer(TValue& value) //传输特殊字段
        {
            if constexpr (requires() { FieldDataTransferrer_Transfer<TValue>::Invoke; })
                FieldDataTransferrer_Transfer<TValue>::Invoke(*this, value); //优先使用自定义实现
            else
                this->FallbackTransfer(&value, typeid(value)); //否则用RTTI处理
        }

        virtual void PushNode(std::optional<std::string_view> name, DataType dataType)
        {
        }
        virtual void PopNode()
        {
        }
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
        virtual void Transfer(std::filesystem::path& path);
        /**
         * 最终失败回退，基于RTTI处理
         * @param value 
         * @param typeIndex 
         */
        virtual void FallbackTransfer(void* value, std::type_index typeIndex);
        virtual void FallbackTransferPtr(std::weak_ptr<void>& value, std::type_index objectTypeIndex)
        {
        }
    };
    static_assert(FieldTransferrer<FieldDataTransferrer>);
}