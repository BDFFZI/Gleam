#pragma once

#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

namespace Gleam
{
    class EditorUI
    {
    public:
        static void DrawSerializedContent(void* target, std::type_index targetType);
        static void DrawEntityField(Entity& entity);
        static void SetDragDropObject(const std::shared_ptr<void>& objectPtr, std::type_index objectTypeIndex);
        static std::shared_ptr<void> GetDragDropObject(std::type_index objectType);
    };
}