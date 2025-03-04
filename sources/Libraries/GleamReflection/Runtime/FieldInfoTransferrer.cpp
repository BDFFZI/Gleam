#include "FieldInfoTransferrer.h"

namespace Gleam
{
    FieldInfoTransferrer::FieldInfoTransferrer(void* target): targetAddress(static_cast<std::byte*>(target))
    {
    }
    std::vector<FieldInfo>& FieldInfoTransferrer::GetResult()
    {
        fieldInfos.shrink_to_fit();
        return fieldInfos;
    }
}