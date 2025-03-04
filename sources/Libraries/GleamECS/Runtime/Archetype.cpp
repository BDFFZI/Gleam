#include "Archetype.h"

#include <cassert>

#include "View.h"
#include "GleamUtility/Runtime/md5.h"

namespace Gleam
{
    Archetype& Archetype::Create(const std::initializer_list<std::reference_wrapper<Type>> componentTypes, const std::string_view name)
    {
        std::vector<const Type*> types = std::vector<const Type*>{componentTypes.size()};
        for (size_t i = 0; i < componentTypes.size(); ++i)
            types[i] = &componentTypes.begin()[i].get();

        Archetype archetype = {name, types};
        return allArchetypes.emplace(archetype.id, std::move(archetype)).first->second;
    }
    Archetype& Archetype::CreateOrGet(const std::initializer_list<std::reference_wrapper<Type>> componentTypes)
    {
        uuids::uuid id = GetID(componentTypes);
        if (allArchetypes.contains(id))
            return allArchetypes.at(id);
        return Create(componentTypes);
    }
    uuids::uuid Archetype::GetID(const std::initializer_list<std::reference_wrapper<Type>> componentTypes)
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
        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->Construct(address);
            address += componentTypes[i]->GetSize();
        }
    }
    void Archetype::Destruct(std::byte* address) const
    {
        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->Destruct(address);
            address += componentTypes[i]->GetSize();
        }
    }
    void Archetype::MoveConstruct(std::byte* source, std::byte* destination) const
    {
        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->MoveConstruct(source, destination);
            source += componentTypes[i]->GetSize();
            destination += componentTypes[i]->GetSize();
        }
    }
    void Archetype::Move(std::byte* source, std::byte* destination) const
    {
        for (int i = 0; i < componentCount; ++i)
        {
            componentTypes[i]->Move(source, destination);
            source += componentTypes[i]->GetSize();
            destination += componentTypes[i]->GetSize();
        }
    }

    Archetype::Archetype(const std::string_view name, std::vector<const Type*> componentTypes)
    {
        //原型与组件排序无关
        std::ranges::sort(
            componentTypes,
            [](auto a, auto b) { return a->GetID() < b->GetID(); }
        );
        std::string componentIDs = {};
        for (const auto& componentType : componentTypes)
            componentIDs += to_string(componentType->GetID());
        MD5 md5 = MD5(componentIDs);

        this->id = uuids::uuid(md5.toArray());
        this->name = name.empty() ? md5.toStr() : name;

        componentTypes.insert(componentTypes.begin(), &Type::CreateOrGetType<Entity>());
        this->componentCount = static_cast<int>(componentTypes.size());
        this->componentMapping.reserve(componentCount);
        this->componentTypes.reserve(componentCount);
        this->componentOffsets.reserve(componentCount);
        this->size = 0;
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
        std::string result = {archetype.GetName()};
        for (int i = 0; i < archetype.GetComponentCount(); ++i)
        {
            result += std::format(
                "\n{:20} {:5} {:5}",
                archetype.GetComponentType(i).GetName(),
                archetype.GetComponentOffset(i),
                archetype.GetComponentType(i).GetSize()
            );
        }
        return result;
    }
}