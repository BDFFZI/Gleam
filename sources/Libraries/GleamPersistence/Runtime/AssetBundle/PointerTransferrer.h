#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"

namespace Gleam
{
    inline std::function<void(FieldDataTransferrer&, void*&, std::type_index)> FieldDataTransferrer_TransferPtrEvent = nullptr;
    /**
     * 将指针传输事件化，以便用户对传输各种指针时进行处理
     * @tparam TValue 
     */
    template <typename TValue>
    struct FieldDataTransferrer_Transfer<TValue*>
    {
        static void Invoke(FieldDataTransferrer& serializer, TValue*& value)
        {
            if (FieldDataTransferrer_TransferPtrEvent != nullptr)
            {
                void** ptr = reinterpret_cast<void**>(&value);
                FieldDataTransferrer_TransferPtrEvent(serializer, *ptr, typeid(TValue));
                value = static_cast<TValue*>(*ptr);
            }
        }
    };

    /**
     * 一个空的字段数据传输器（不会进行数据传递），用于实现仅触发指针传输事件。
     */
    class NullTransferrer : public FieldDataTransferrer
    {
        void PushNode(std::optional<std::string_view> name, DataType dataType) override
        {
        }
        void PopNode() override
        {
        }
        void Transfer(double& value) override
        {
        }
        void Transfer(int64_t& value) override
        {
        }
        void Transfer(std::string& value) override
        {
        }
        void Transfer(std::vector<std::byte>& value) override
        {
        }
    };
}