#pragma once

#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    using CustomUI = std::unordered_map<std::type_index, std::function<void(void*)>>;
    class EditorUI
    {
    public:
        static void DrawSerializedContent(void* target, std::type_index targetType);
        static void DrawEntityField(Entity entity);
        static void DrawObjectField(void* object, std::type_index type)
        {
        }

    private:
        inline static Entity dragging = Entity::Null;
        inline static Entity inspecting = Entity::Null;
    };
}