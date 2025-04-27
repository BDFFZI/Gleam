#include "EntityAsset.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<EntityAsset>> EntityAsset::GetEntityAsset(const Entity entity)
    {
        return entityToAsset.contains(entity)
                   ? std::optional<std::reference_wrapper<EntityAsset>>{*entityToAsset.at(entity)}
                   : std::nullopt;
    }

    EntityAsset::EntityAsset(): linkedEntity(Entity::Null), allowRemoveEntity(false)
    {
    }
    EntityAsset::EntityAsset(const Entity entity, const bool ownership): linkedEntity(entity), allowRemoveEntity(ownership)
    {
        if (entity != Entity::Null)
            entityToAsset[entity] = this;
    }
    EntityAsset::EntityAsset(EntityAsset&& other) noexcept
    {
        linkedEntity = other.linkedEntity;
        allowRemoveEntity = other.allowRemoveEntity;

        other.linkedEntity = Entity::Null;
        other.allowRemoveEntity = false;

        if (linkedEntity != Entity::Null)
            entityToAsset[linkedEntity] = this;
    }
    EntityAsset& EntityAsset::operator=(EntityAsset&& other) noexcept
    {
        assert(linkedEntity != other.linkedEntity && "不能用自身移动赋值！");
        assert(other.linkedEntity != Entity::Null && "用于赋值的实体是空的！");

        if (linkedEntity == Entity::Null)
        {
            linkedEntity = other.linkedEntity;
            allowRemoveEntity = other.allowRemoveEntity;

            other.linkedEntity = Entity::Null;
            other.allowRemoveEntity = false;

            if (linkedEntity != Entity::Null)
                entityToAsset[linkedEntity] = this;
        }
        else if (other.linkedEntity != Entity::Null)
        {
            const EntityInfo& otherEntityInfo = World::GetEntityInfoAllocator().GetEntityInfo(other.linkedEntity);
            const EntityInfo& entityInfo = World::GetEntityInfoAllocator().GetEntityInfo(linkedEntity);
            if (entityInfo.archetype != otherEntityInfo.archetype)
                World::GetEntityAllocator().MoveEntity(linkedEntity, *otherEntityInfo.archetype);
            otherEntityInfo.archetype->Move(entityInfo.memoryAddress, otherEntityInfo.memoryAddress);
        }

        return *this;
    }
    EntityAsset::~EntityAsset()
    {
        if (linkedEntity != Entity::Null)
        {
            entityToAsset.erase(linkedEntity);
            ///Entity所有权始终在World手中，虽然System内也可以调整Entity，但World确保了System的生命周期在Entity之内，故即使用户控制也不会有事
            ///但EntityAsset的生命周期跟随资源包，这超出了World中Entity的生命周期。为了避免所有权冲突，故将EntityAsset视作Entity的弱引用。
            ///因此EntityAsset只能尝试回收Entity，即回收前必须判断存在。
            if (allowRemoveEntity && World::GetEntityInfoAllocator().HasEntity(linkedEntity))
                World::RemoveSceneEntityAsync(linkedEntity);
        }
    }

    Entity EntityAsset::GetLinkedEntity() const
    {
        return linkedEntity;
    }
    bool EntityAsset::GetAllowRemoveEntity() const
    {
        return allowRemoveEntity;
    }
    void EntityAsset::SetLinkedEntity(const Entity entity)
    {
        if (this->linkedEntity != Entity::Null)
            entityToAsset.erase(this->linkedEntity);

        this->linkedEntity = entity;

        if (this->linkedEntity != Entity::Null)
            entityToAsset[this->linkedEntity] = this;
    }
    void EntityAsset::SetAllowRemoveEntity(const bool ownership)
    {
        this->allowRemoveEntity = ownership;
    }
}