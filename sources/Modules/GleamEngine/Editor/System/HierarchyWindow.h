#pragma once
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    class HierarchyWindow : public System
    {
    public:
        HierarchyWindow(): System(GlobalEditorUISystem)
        {
        }

        static void DrawEntity(Entity entity);
        static void DrawSystem(System& system);
        static void DrawSubSystems(SystemGroup& systemGroup);

        static void DrawSystemsPopup();
        static void DrawEntitiesPopup();
        static void DrawSystemPopup(System& system);
        static void DrawEntityPopup(Entity entity);

        static void DrawWorld();
        static void DrawWorldUnfolding();
    
    private:
        inline static std::vector<Entity> removingEntities = {};
        inline static std::vector<System*> removingSystems = {};


        void Update() override;
    };
    Gleam_MakeGlobalSystem(HierarchyWindow)
}