#pragma once
#include "EditorUISystem.h"

namespace Gleam
{
    class HierarchyWindow : public System<EditorUISystem>
    {
    public:
        static bool DrawSystem(const ISystemEvent& system);
        static void DrawSubSystems(const ISystemGroup& systemGroup);
        static bool DrawSystemPopup(const ISystemEvent& system);
        static void DrawSystemsPopup();

        static bool DrawEntity(Entity entity);
        static bool DrawEntityPopup(Entity entity);
        static void DrawEntitiesPopup();

        static void DrawWorld();
        static void DrawWorldUnfolding();

    private:
        void Update() override;
    };
    Gleam_MakeEditorSystem(HierarchyWindow)
}