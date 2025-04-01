#pragma once
#include "GleamReflection/Runtime/FieldDataTransferrer.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamWindow/Runtime/InputEnum.h"

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
        Asset(const std::shared_ptr<void>& object, const Type& objectType);
        template <class T> requires !std::is_reference_v<T>
        Asset(T&& data)
        {
            const Type& type = Type::GetType(typeid(T)).value();
            objectType = &type;
            objectPtr = type.MakeShared(type.Create());
            type.Move(objectPtr.get(), &data);
        }
        Asset(Asset&& other) noexcept;
        Asset& operator=(Asset&& other) noexcept;


        std::shared_ptr<void>& GetObjectPtr();
        const Type& GetObjectType() const;
        template <class T>
        T& GetObject()
        {
            return *static_cast<T*>(objectPtr.get());
        }
        template <class T>
        std::shared_ptr<T> GetObjectPtr()
        {
            return std::shared_ptr<T>(objectPtr, static_cast<T*>(objectPtr.get()));
        }

    private:
        Gleam_MakeType_Friend

        std::shared_ptr<void> objectPtr;
        const Type* objectType;
    };

    Gleam_MakeTypeWithID(Asset, "E21E1632-F550-4A06-AC51-08221E4A6E9D")
    {
        if constexpr (std::derived_from<TFieldTransferrer, FieldDataTransferrer>)
        {
            uuids::uuid typeID = value.objectType != nullptr ? value.objectType->GetID() : uuids::uuid{};
            transferrer.TransferField("typeID", typeID);
            value.objectType = &Type::GetType(typeID).value().get();

            const Type& type = *value.objectType;

            if (value.objectPtr == nullptr) //反持久化
            {
                value.objectPtr = type.MakeShared(type.Create());
            }

            transferrer.PushNode("data", DataType::Class);
            type.Serialize(transferrer, value.objectPtr.get());
            transferrer.PopNode();
        }
        else
        {
            Gleam_MakeType_AddField(objectPtr);
            Gleam_MakeType_AddField(objectType);
        }
    }
}