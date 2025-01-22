#pragma once
#include <string>
#include <typeindex>
#include <vector>

#include "Transferrer.h"

namespace Light
{
    struct FieldInfo
    {
        std::string name;
        std::type_index type;
        std::ptrdiff_t offset;
        std::size_t size;
    };

    class MemberTransferrer
    {
    public:
        MemberTransferrer(void* target)
            : targetAddress(static_cast<std::byte*>(target))
        {
        }

        std::vector<FieldInfo>& GetResult()
        {
            fieldInfos.shrink_to_fit();
            return fieldInfos;
        }

        template <class TMember>
        void TransferField(const char* name, TMember& value)
        {
            fieldInfos.emplace_back(
                name,
                typeid(TMember),
                reinterpret_cast<std::byte*>(&value) - targetAddress,
                sizeof(TMember)
            );
        }

    private:
        std::byte* targetAddress;
        std::vector<FieldInfo> fieldInfos;
    };
    static_assert(Transferrer<MemberTransferrer>);
}