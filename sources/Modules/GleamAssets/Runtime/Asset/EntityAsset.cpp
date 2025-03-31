#include "EntityAsset.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<EntityAsset>> EntityAsset::GetEntityAsset(const Entity entity)
    {
        return entityToAsset.contains(entity)
                   ? std::optional<std::reference_wrapper<EntityAsset>>{*entityToAsset.at(entity)}
                   : std::nullopt;
    }

    EntityAsset::EntityAsset(): entity(Entity::Null), ownership(false)
    {
    }
    EntityAsset::EntityAsset(const Entity entity, const bool ownership): entity(entity), ownership(ownership)
    {
        if (entity != Entity::Null)
            entityToAsset[entity] = this;
    }
    EntityAsset::EntityAsset(EntityAsset&& other) noexcept
    {
        entity = other.entity;
        ownership = other.ownership;

        other.entity = Entity::Null;
        other.ownership = false;

        if (entity != Entity::Null)
            entityToAsset[entity] = this;
    }
    EntityAsset& EntityAsset::operator=(EntityAsset&& other) noexcept
    {
        assert(entity != other.entity && "不能用自身移动赋值！");
        assert(other.entity != Entity::Null && "用于赋值的实体是空的！");

        if (entity == Entity::Null)
        {
            entity = other.entity;
            ownership = other.ownership;

            other.entity = Entity::Null;
            other.ownership = false;

            if (entity != Entity::Null)
                entityToAsset[entity] = this;
        }
        else if (other.entity != Entity::Null)
        {
            const EntityInfo& otherEntityInfo = World::GetEntityInfoAllocator().GetEntityInfo(other.entity);
            const EntityInfo& entityInfo = World::GetEntityInfoAllocator().GetEntityInfo(entity);
            if (entityInfo.archetype != otherEntityInfo.archetype)
                World::MoveEntity(entity, *otherEntityInfo.archetype);
            otherEntityInfo.archetype->Move(entityInfo.memoryAddress, otherEntityInfo.memoryAddress);
        }

        return *this;
    }
    EntityAsset::~EntityAsset()
    {
        if (entity != Entity::Null)
        {
            entityToAsset.erase(entity);
            if (ownership)
                World::RemoveEntityAsync(entity);
        }
    }

    Entity EntityAsset::GetEntity() const
    {
        return entity;
    }
    bool EntityAsset::GetOwnership() const
    {
        return ownership;
    }
    void EntityAsset::SetEntity(const Entity entity)
    {
        if (this->entity != Entity::Null)
            entityToAsset.erase(this->entity);

        this->entity = entity;

        if (this->entity != Entity::Null)
            entityToAsset[this->entity] = this;
    }
    void EntityAsset::SetOwnership(const bool ownership)
    {
        this->ownership = ownership;
    }
}