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
        static Entity GetEntityDrawing();

        static void DrawEntity(Entity entity);
        static void DrawSystem(System& system);
        static void DrawSubSystems(SystemGroup& systemGroup);

        static void DrawWorld();
        static void DrawWorldUnfolding();

        static void DrawDefaultContent(void* target, std::type_index targetType);
        static void DrawEntityContent(Entity entity, const CustomUI& componentGUI = {}, bool pure = false);

    private:
        inline static Entity drawing = Entity::Null;
        inline static Entity dragging = Entity::Null;
        inline static Entity inspecting = Entity::Null;
    };
}