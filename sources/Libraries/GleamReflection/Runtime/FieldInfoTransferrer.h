#pragma once
#include <string>
#include <typeindex>
#include <vector>

#include "FieldTransferrer.h"

namespace Gleam
{
    struct FieldInfo
    {
        std::string name;
        std::type_index type;
        int offset;
        int size;
    };

    /**
     * 字段信息传输器，用于统计对象的成员字段信息。
     */
    class FieldInfoTransferrer
    {
    public:
        FieldInfoTransferrer(void* target);

        std::vector<FieldInfo>& GetResult();

        template <class TMember>
        void TransferField(const char* name, TMember& value)
        {
            fieldInfos.emplace_back(
                name,
                typeid(TMember),
                static_cast<int>(reinterpret_cast<std::byte*>(&value) - targetAddress),
                static_cast<int>(sizeof(TMember))
            );
        }

    private:
        std::byte* targetAddress;
        std::vector<FieldInfo> fieldInfos;
    };
    static_assert(FieldTransferrer<FieldInfoTransferrer>);
}