#pragma once

#include "GleamECS/Runtime/Entity.h"
#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    using CustomGUI = std::unordered_map<std::type_index, std::function<void(void*)>>;
    class EditorUI
    {
    public:
        static Entity GetEntityDrawing();
        static void DrawEntityButton(Entity entity);
        static void DrawEntity(Entity entity, const CustomGUI& componentGUI = {});
        static void DrawEntityPure(Entity entity, const CustomGUI& componentGUI);
        static bool DrawSystem(System* system);
        static void DrawSystemGroup(SystemGroup* systemGroup);
        static void DrawSystemGroupContent(SystemGroup* systemGroup);
        static void DrawWorld();
        static void DrawWorldUnfolding();

    private:
        inline static Entity drawing = Entity::Null;
        inline static Entity dragging = Entity::Null;
    };
}