#pragma once
#include "World.h"
#include "_Concept.hpp"

namespace Light
{
    template <Component... TComponent>
    struct Reference
    {
        Entity target = Entity::Null;

        Reference() = default;
        Reference(const Entity target) : target(target)
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
            World::CopyComponents(target, components...);
        }
        void Set(const TComponent&... components)
        {
            World::PasteComponents(target, components...);
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
