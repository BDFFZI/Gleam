#pragma once
#include "GleamReflection/Runtime/Transferrer/DataTransferrer.h"

namespace Gleam
{
    class Asset
    {
    public:
        Asset();
        Asset(int id, uuids::uuid type, void* data);
        Asset(Asset&& asset) noexcept;
        Asset& operator=(Asset&& asset) noexcept;
        ~Asset();

        uuids::uuid GetTypeID() const;
        void* GetData() const;
    private:
        Gleam_MakeType_Friend
        friend class AssetBundle;

        int id;
        uuids::uuid typeID;
        void* data;
        bool ownership;
    };

    Gleam_MakeType(Asset, "")
    {
        Gleam_MakeType_AddField(id);
        Gleam_MakeType_AddField(typeID);

        if constexpr (std::derived_from<TTransferrer, DataTransferrer>)
        {
            auto optionalType = Type::GetType(value.typeID);
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