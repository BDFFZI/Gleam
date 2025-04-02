#include "PersistentEntity.h"

namespace Gleam
{
    std::optional<std::reference_wrapper<PersistentEntity>> PersistentEntity::GetEntityAsset(const Entity entity)
    {
        return entityToAsset.contains(entity)
                   ? std::optional<std::reference_wrapper<PersistentEntity>>{*entityToAsset.at(entity)}
                   : std::nullopt;
    }

    PersistentEntity::PersistentEntity(): entity(Entity::Null), ownership(false)
    {
    }
    PersistentEntity::PersistentEntity(const Entity entity, const bool ownership): entity(entity), ownership(ownership)
    {
        if (entity != Entity::Null)
            entityToAsset[entity] = this;
    }
    PersistentEntity::PersistentEntity(PersistentEntity&& other) noexcept
    {
        entity = other.entity;
        ownership = other.ownership;

        other.entity = Entity::Null;
        other.ownership = false;

        if (entity != Entity::Null)
            entityToAsset[entity] = this;
    }
    PersistentEntity& PersistentEntity::operator=(PersistentEntity&& other) noexcept
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
    PersistentEntity::~PersistentEntity()
    {
        if (entity != Entity::Null)
        {
            entityToAsset.erase(entity);
            if (ownership)
                World::RemoveEntityAsync(entity);
        }
    }

    Entity PersistentEntity::GetEntity() const
    {
        return entity;
    }
    bool PersistentEntity::GetOwnership() const
    {
        return ownership;
    }
    void PersistentEntity::SetEntity(const Entity entity)
    {
        if (this->entity != Entity::Null)
            entityToAsset.erase(this->entity);

        this->entity = entity;

        if (this->entity != Entity::Null)
            entityToAsset[this->entity] = this;
    }
    void PersistentEntity::SetOwnership(const bool ownership)
    {
        this->ownership = ownership;
    }
}