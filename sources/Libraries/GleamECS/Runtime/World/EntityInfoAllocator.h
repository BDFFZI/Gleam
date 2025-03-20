#pragma once
#include "GleamECS/Runtime/Archetype.h"

namespace Gleam
{
    /**
     * 对每个实体存储的额外信息，如地址原型信息。
     * 用于实现反向查找，从而像面对对象一样访问实体。
     */
    struct EntityInfo
    {
        const Archetype* archetype;
        class EntityAllocator* allocator;
        int memoryIndex;
        std::byte* memoryAddress;

        EntityInfo(const Archetype& archetype, EntityAllocator& allocator, int memoryIndex, std::byte* memoryAddress);
        EntityInfo() = default;
    };

    class EntityInfoAllocator
    {
    public:
        Entity NextEntity();
        bool HasEntity(Entity entity) const;
        const EntityInfo& GetEntityInfo(Entity entity) const;
        void SetEntityInfo(Entity entity, const std::optional<EntityInfo>& info);
        void Clear();

    private:
        friend class HierarchyWindow;

        uint32_t nextEntity = 1;
        std::unordered_map<Entity, EntityInfo> entityInfos = {};
    };
}