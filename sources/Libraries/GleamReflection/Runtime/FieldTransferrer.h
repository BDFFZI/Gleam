#pragma once

namespace Gleam
{
    /**
     * 字段传输器。支持传递对象的任意类型成员字段。
     */
    template <class TTransferrer>
    concept FieldTransferrer = requires(TTransferrer& transferrer, void* field) { transferrer.TransferField("fieldName", field); };

    /**
     * 自定义对象的成员字段传输方式。
     * @tparam T 
     * @tparam TFieldTransferrer 
     */
    template <FieldTransferrer TFieldTransferrer, typename T>
    struct TransferObjectField
    {
        static void Invoke(TFieldTransferrer& transferrer, T& object)
        {
        }
    };
}