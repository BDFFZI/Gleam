#pragma once
#include "World.h"
#include "Entity.h"

namespace Light
{
    template <Component... TComponent>
    struct EntityRef
    {
        Entity target = Entity::Null;

        EntityRef() = default;
        EntityRef(const Entity target) : target(target)
        {
        }
        operator Entity() const
        {
            return target;
        }

        void Get(TComponent**... components)
        {
            World::GetComponents(target, components...);
        }
        void Get(TComponent*... components)
        {
            World::GetComponents(target, components...);
        }
        void Set(const TComponent&... components)
        {
            World::SetComponents(target, components...);
        }

        bool operator==(const Entity entity) const
        {
            return target == entity;
        }
        bool operator!=(const Entity entity) const
        {
            return target != entity;
        }
    };
}
