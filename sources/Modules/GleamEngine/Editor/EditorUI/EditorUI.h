#pragma once

#include "GleamECS/Runtime/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    class EditorUI
    {
    public:
        static void DrawSerializedContent(void* target, std::type_index targetType);
        static void DrawEntityField(Entity& entity);
        static void DrawObjectField(void* object, std::type_index type)
        {
        }

    private:
        inline static Entity inspecting = Entity::Null;
    };
}