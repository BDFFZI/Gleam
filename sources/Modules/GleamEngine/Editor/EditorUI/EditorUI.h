#pragma once

#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    using CustomUI = std::unordered_map<std::type_index, std::function<void(void*)>>;
    class EditorUI
    {
    public:
        static Entity GetEntityDrawing();
        static void DrawEntityButton(Entity entity);
        static void DrawEntity(Entity entity, const CustomUI& componentGUI = {});
        static void DrawEntityPure(Entity entity, const CustomUI& componentGUI);
        static bool DrawSystem(System& system);
        static void DrawSystemGroup(SystemGroup& systemGroup);
        static void DrawSystemGroupContent(SystemGroup& systemGroup);
        static void DrawWorld();
        static void DrawWorldUnfolding();
        static void DrawDefaultInspectorUI(void* target,std::type_index targetType);

    private:
        inline static Entity drawing = Entity::Null;
        inline static Entity dragging = Entity::Null;
        inline static Entity inspecting = Entity::Null;
    };
}