#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"
#include "GleamReflection/Runtime/Type.h"

#undef GetObject

namespace Gleam
{
    /**
     * 对象持久化包装器。
     *
     * 对象必须转换为Asset才可以被持久化。
     *
     * 1. 当一个对象实现了反射并正确注册了序列化函数，便可支持持久化。
     * 2. Asset将自动根据对象的序列化函数，持久化或反持久化对象，并托管因此生成的对象。
     */
    class Asset
    {
    public:
        Asset() = default;
        Asset(void* object, const Type& objectType, bool ownership);
        template <class T> requires !std::is_reference_v<T>
        Asset(T&& data)
        {
            const Type& type = Type::GetType(typeid(T)).value();
            objectType = &type;
            object = type.Create();
            ownership = true;
            type.Move(object, &data);
        }
        Asset(Asset&& asset) noexcept;
        Asset& operator=(Asset&& asset) noexcept;
        ~Asset();

        void* GetObject() const;
        const Type& GetObjectType() const;
        bool GetOwnership() const;
        void SetOwnership(bool ownership);

    private:
        Gleam_MakeType_Friend

        void* object;
        const Type* objectType;
        bool ownership;
    };

    Gleam_MakeType(Asset, "E21E1632-F550-4A06-AC51-08221E4A6E9D")
    {
        if constexpr (std::derived_from<TFieldTransferrer, FieldDataTransferrer>)
        {
            uuids::uuid typeID = value.objectType != nullptr ? value.objectType->GetID() : uuids::uuid{};
            transferrer.TransferField("typeID", typeID);
            value.objectType = &Type::GetType(typeID).value().get();

            const Type& type = *value.objectType;

            if (value.object == nullptr) //反持久化
            {
                value.object = type.Create();
                value.ownership = true;
            }

            transferrer.PushNode("data", DataType::Class);
            type.Serialize(transferrer, value.object);
            transferrer.PopNode();
        }
        else
        {
            Gleam_MakeType_AddField(object);
            Gleam_MakeType_AddField(objectType);
            Gleam_MakeType_AddField(ownership);
        }
    }
}