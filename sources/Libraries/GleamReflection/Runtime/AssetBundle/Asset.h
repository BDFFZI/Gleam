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

        int GetID() const;
        uuids::uuid GetTypeID() const;
        void* GetDataRef() const;
    private:
        Gleam_MakeType_Friend
        friend class AssetBundle;

        int id;
        uuids::uuid typeID;
        void* dataRef;
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
                if (value.dataRef == nullptr)
                {
                    value.dataRef = std::malloc(type.GetSize());
                    type.GetConstruct()(value.dataRef);
                    value.ownership = true;
                }

                transferrer.PushNode("data", DataType::Class);
                type.GetSerialize()(transferrer, value.dataRef);
                transferrer.PopNode();
            }
        }
        else
        {
            Gleam_MakeType_AddField(dataRef);
        }
    }
}