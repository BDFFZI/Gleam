#pragma once

#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/Scene.h"
#include "LightECS/Runtime/System.h"

namespace Light
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
        static void DrawScene(Scene* scene);
        static void DrawSceneUnfolding(Scene* scene);

    private:
        inline static Entity drawing = Entity::Null;
        inline static Entity dragging = Entity::Null;
    };
}