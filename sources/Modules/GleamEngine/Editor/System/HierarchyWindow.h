#pragma once
#include "GleamUI/Runtime/UI.h"
#include "EditorUISystem.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    class HierarchyWindow : public System
    {
    public:
        HierarchyWindow(): System(GlobalEditorUISystem)
        {
        }

        static bool DrawEntity(Entity entity);
        static bool DrawSystem(System& system);
        static void DrawSubSystems(SystemGroup& systemGroup);

        static void DrawSystemsPopup();
        static void DrawEntitiesPopup();
        static bool DrawSystemPopup(System& system);
        static bool DrawEntityPopup(Entity entity);

        static void DrawWorld();
        static void DrawWorldUnfolding();

    private:
        void Update() override;
    };
    Gleam_MakeSystem(HierarchyWindow)
}