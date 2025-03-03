#include "Type.h"

#include "GleamUtility/Runtime/md5.h"

namespace Gleam
{
    Type& Type::Create(const std::initializer_list<std::reference_wrapper<const Type>> components)
    {
        std::string componentUuids;
        for (auto& component : components)
            componentUuids += to_string(component.get().GetID());
        uuids::uuid typeID = uuids::uuid::from_string(MD5(componentUuids).toStr()).value();

        assert(!allTypes.contains(typeID) && "已有相同编号的类型！");
        Type& type = allTypes.insert({typeID, Type{}}).first->second;
        type.id = typeID;
        type.size = 0;
        for (auto& component : components)
            type.size += component.get().GetSize();
        type.parent = std::nullopt;
        //合并字段
        type.fields = {};
        for (auto& component : components)
            type.fields.insert(type.fields.end(), component.get().fields.begin(), component.get().fields.end());
        int offset = 0;
        for (auto& field : type.fields)
        {
            field.offset = offset;
            offset += static_cast<int>(field.size);
        }

        type.construct = [components](void* ptr)
        {
            std::byte* address = static_cast<std::byte*>(ptr);
            for (auto component : components)
            {
                component.get().GetConstruct()(address);
                address += component.get().GetSize();
            }
        };
        type.destruct = [components](void* ptr)
        {
            std::byte* address = static_cast<std::byte*>(ptr);
            for (auto component : components)
            {
                component.get().GetDestruct()(address);
                address += component.get().GetSize();
            }
        };
        type.move = [components](void* source, void* destination)
        {
            std::byte* sourceAddress = static_cast<std::byte*>(source);
            std::byte* destinationAddress = static_cast<std::byte*>(destination);
            for (auto component : components)
            {
                component.get().GetMove()(sourceAddress, destinationAddress);
                sourceAddress += component.get().GetSize();
                destinationAddress += component.get().GetSize();
            }
        };

        //序列化
        type.serialize = [components](DataTransferrer& serializer, void* ptr)
        {
            std::byte* address = static_cast<std::byte*>(ptr);
            for (auto component : components)
            {
                component.get().GetSerialize()(serializer, address);
                address += component.get().GetSize();
            }
        };

        return type;
    }
    std::optional<std::reference_wrapper<Type>> Type::GetType(const uuids::uuid typeID)
    {
        if (allTypes.contains(typeID) == false)
            return std::nullopt;
        return allTypes.at(typeID);
    }
    std::optional<std::reference_wrapper<Type>> Type::GetType(const std::type_index typeIndex)
    {
        if (typeIndexToID.contains(typeIndex) == false)
            return std::nullopt;
        return GetType(typeIndexToID.at(typeIndex));
    }

    uuids::uuid Type::GetID() const
    {
        return id;
    }
    size_t Type::GetSize() const
    {
        return size;
    }
    std::optional<std::reference_wrapper<const Type>> Type::GetParent() const
    {
        return parent;
    }
    const std::vector<FieldInfo>& Type::GetFields() const
    {
        return fields;
    }
    const std::function<void(void*)>& Type::GetConstruct() const
    {
        return construct;
    }
    const std::function<void(void*)>& Type::GetDestruct() const
    {
        return destruct;
    }
    const std::function<void(void*, void*)>& Type::GetMove() const
    {
        return move;
    }
    const std::function<void(DataTransferrer&, void*)>& Type::GetSerialize() const
    {
        return serialize;
    }
}