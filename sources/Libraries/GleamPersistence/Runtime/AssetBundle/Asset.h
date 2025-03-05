#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class Asset
    {
    public:
        Asset();
        Asset(int id, uuids::uuid typeID, void* dataRef);
        template <class T> requires !std::is_reference_v<T>
        Asset(const int id, T&& data)
            : id(id)
        {
            const Type& type = Type::GetType(typeid(T)).value();
            typeID = type.GetID();
            dataRef = type.Create();
            ownership = true;
            type.Move(dataRef, &data);
        }
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

    Gleam_MakeType(Asset, "1CEF16B4-DF11-41B0-A848-9221ABE9803B")
    {
        Gleam_MakeType_AddField(id);
        Gleam_MakeType_AddField(typeID);

        if constexpr (std::derived_from<TFieldTransferrer, FieldDataTransferrer>)
        {
            auto optionalType = Type::GetType(value.typeID);
            if (optionalType.has_value())
            {
                const Type& type = optionalType.value().get();
                if (value.dataRef == nullptr) //反持久化
                {
                    value.dataRef = type.Create();
                    value.ownership = true;
                }

                transferrer.PushNode("data", DataType::Class);
                type.Serialize(transferrer, value.dataRef);
                transferrer.PopNode();
            }
        }
        else
        {
            Gleam_MakeType_AddField(dataRef);
        }
    }
}