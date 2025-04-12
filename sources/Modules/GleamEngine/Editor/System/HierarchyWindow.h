#pragma once
#include "EditorUISystem.h"

namespace Gleam
{
    class HierarchyWindow : public System<EditorUISystem>
    {
    public:
        static bool DrawEntity(Entity entity);
        static bool DrawSystem(const ISystemEvent& system);
        static void DrawSubSystems(const ISystemGroup& systemGroup);

        static void DrawSystemsPopup();
        static void DrawEntitiesPopup();
        static bool DrawSystemPopup(const ISystemEvent& system);
        static bool DrawEntityPopup(Entity entity);

        static void DrawWorld();
        static void DrawWorldUnfolding();

    private:
        void Update() override;
    };
    Gleam_MakeEditorSystem(HierarchyWindow)
}