#pragma once
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class HierarchyWindow : public System
    {
    public:
        HierarchyWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        inline static std::vector<Entity> removingEntities = {};
        inline static std::vector<System*> removingSystems = {};

        static void DrawEntity(Entity entity);
        static void DrawSystem(System& system);
        static void DrawSubSystems(SystemGroup& systemGroup);

        static void DrawSystemsPopup(std::optional<std::reference_wrapper<Scene>> scene);
        static void DrawEntitiesPopup(std::optional<std::reference_wrapper<Scene>> scene);
        static void DrawSystemPopup(System& system);
        static void DrawEntityPopup(Entity entity);

        static void DrawWorld();
        static void DrawWorldUnfolding();
        static void DrawScene(Scene& scene);

        void Update() override;
    };
    Gleam_MakeGlobalSystem(HierarchyWindow)
}