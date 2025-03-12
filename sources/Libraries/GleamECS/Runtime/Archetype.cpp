#include "Archetype.h"

#include <cassert>

#include "View.h"
#include "GleamUtility/Runtime/md5.h"

namespace Gleam
{
    Archetype& Archetype::Create(const std::vector<std::reference_wrapper<const Type>>& componentTypes, const std::string_view name)
    {
        std::vector<const Type*> types;
        types.reserve(componentTypes.size());
        for (const auto& type : componentTypes)
            types.push_back(&type.get());

        Archetype archetype = {name, types};
        return allArchetypes.emplace(archetype.id, std::move(archetype)).first->second;
    }
    Archetype& Archetype::CreateOrGet(const std::vector<std::reference_wrapper<const Type>>& componentTypes)
    {
        uuids::uuid id = GetID(componentTypes);
        if (allArchetypes.contains(id))
            return allArchetypes.at(id);
        return Create(componentTypes);
    }
    uuids::uuid Archetype::GetID(const std::vector<std::reference_wrapper<const Type>>& componentTypes)
    {
        std::string componentIDs = {};
        for (const auto& componentType : componentTypes)
            componentIDs += to_string(componentType.get().GetID());
        return MD5(componentIDs).toArray();
    }

    const std::string& Archetype::GetName() const
    {
        return name;
    }
    uuids::uuid Archetype::GetID() const
    {
        return id;
    }
    int Archetype::GetSize() const
    {
        return size;
    }
    void Archetype::GetComponentTypes(std::vector<std::reference_wrapper<const Type>>& result) const
    {
        result.clear();
        for (int i = 0; i < componentCount; ++i)
            result.emplace_back(GetComponentType(i));
    }

    bool Archetype::HasComponent(const std::type_index component) const
    {
        return componentMapping.contains(component);
    }
    int Archetype::GetComponentCount() const
    {
        return componentCount;
    }
    const Type& Archetype::GetComponentType(const int index) const
    {
        return *componentTypes[index];
    }
    int Archetype::GetComponentOffset(const int index) const
    {
        return componentOffsets[index];
    }
    int Archetype::GetComponentOffset(const std::type_index component) const
    {
        assert(componentMapping.contains(component) && "此原型不包含目标组件！");

        return componentOffsets[componentMapping.at(component)];
    }


    void Archetype::Construct(std::byte* address) const
    {
        address += sizeof(Entity);

        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->Construct(address);
            address += componentTypes[i]->GetSize();
        }
    }
    void Archetype::Destruct(std::byte* address) const
    {
        address += sizeof(Entity);

        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->Destruct(address);
            address += componentTypes[i]->GetSize();
        }
    }
    void Archetype::MoveConstruct(std::byte* destination, std::byte* source) const
    {
        std::memcpy(destination, source, sizeof(Entity));
        destination += sizeof(Entity);
        source += sizeof(Entity);

        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->MoveConstruct(destination, source);
            destination += componentTypes[i]->GetSize();
            source += componentTypes[i]->GetSize();
        }
    }
    void Archetype::Move(std::byte* destination, std::byte* source) const
    {
        destination += sizeof(Entity);
        source += sizeof(Entity);
        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->Move(destination, source);
            destination += componentTypes[i]->GetSize();
            source += componentTypes[i]->GetSize();
        }
    }

    Archetype::Archetype(const std::string_view name, std::vector<const Type*> componentTypes)
    {
        //原型与组件排序无关
        std::ranges::sort(
            componentTypes,
            [](auto a, auto b) { return a->GetID() < b->GetID(); }
        );
        assert(std::ranges::unique(componentTypes).begin() == componentTypes.end() && "一个原型内不允许有重复组件！");

        //计算原型ID
        std::string componentIDs = {};
        for (const auto& componentType : componentTypes)
            componentIDs += to_string(componentType->GetID());
        MD5 md5 = MD5(componentIDs);

        this->id = uuids::uuid(md5.toArray());
        this->name = name.empty() ? md5.toStr() : name;

        this->componentCount = static_cast<int>(componentTypes.size());
        this->componentMapping.reserve(componentCount);
        this->componentTypes.reserve(componentCount);
        this->componentOffsets.reserve(componentCount);
        this->size = sizeof(Entity); //首地址必须留着存储Entity
        for (int i = 0; i < componentCount; ++i)
        {
            const Type& componentType = *componentTypes[i];
            this->componentMapping[componentType.GetIndex()] = i;
            this->componentTypes.push_back(&componentType);
            this->componentOffsets.push_back(this->size);
            this->size += componentType.GetSize();
        }
    }

    std::string to_string(const Archetype& archetype)
    {
        static std::stringstream result = {};

        result.str("");
        result << "Name:" << archetype.GetName() << "\n";
        result << "ID:" << to_string(archetype.GetID()) << "\n";
        result << "Size:" << std::to_string(archetype.GetSize()) << "\n";
        result << "Components:";
        for (int i = 0; i < archetype.GetComponentCount(); ++i)
        {
            result << "\n" << archetype.GetComponentType(i).GetName() << "\n";
            result << std::format(
                "{}\t{}",
                archetype.GetComponentOffset(i),
                archetype.GetComponentType(i).GetSize()
            );
        }
        return result.str();
    }
}