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
        static void SetDragDropObject(void* object, const Type& objectType);
        static void* GetDragDropObject(const Type& objectType);

    private:
        inline static Entity inspecting = Entity::Null;
    };
}