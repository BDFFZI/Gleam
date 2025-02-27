#pragma once
#include "GleamReflection/Runtime/Transferrer/DataTransferrer.h"

namespace Gleam
{
    struct Asset
    {
        int id;
        uuids::uuid type;
        void* data = nullptr;
        bool ownership = false;

        ~Asset()
        {
            if (ownership)
            {
                Type::GetType(type).value().get().GetDestruct()(data);
                std::free(data);
            }
        }
    };

    Gleam_MakeType(Asset, "")
    {
        Gleam_MakeType_AddField(id);
        Gleam_MakeType_AddField(type);

        if constexpr (std::derived_from<TTransferrer, DataTransferrer>)
        {
            auto optionalType = Type::GetType(value.type);
            if (optionalType.has_value())
            {
                Type& type = optionalType.value().get();
                if (value.data == nullptr)
                {
                    value.data = std::malloc(type.GetSize());
                    type.GetConstruct()(value.data);
                    value.ownership = true;
                }

                transferrer.PushNode("data", DataType::Class);
                type.GetSerialize()(transferrer, value.data);
                transferrer.PopNode();
            }
        }
        else
        {
            Gleam_MakeType_AddField(data);
        }
    }
}