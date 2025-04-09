#pragma once
#include "EditorUISystem.h"

namespace Gleam
{
    class HierarchyWindow : public SystemT<EditorUISystem>
    {
    public:
        static bool DrawEntity(Entity entity);
        static bool DrawSystem(SystemAllocator& allocator, System& system);
        static void DrawSubSystems(SystemAllocator& allocator, SystemGroup& systemGroup);

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