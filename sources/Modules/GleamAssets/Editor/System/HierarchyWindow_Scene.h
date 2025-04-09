#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Editor/System/HierarchyWindow.h"

namespace Gleam
{
    class HierarchyWindow_Scene : public System
    {
    public:
        HierarchyWindow_Scene(): System(GlobalHierarchyWindow, SystemRelation::After)
        {
        }

        static void DrawScene(Scene& scene);
        static void DrawScenePopup(Scene& scene);
        static void DrawSceneSystemsPopup(Scene& scene);
        static void DrawSceneEntitiesPopup(Scene& scene);

    private:
        inline static std::vector<Scene*> removingScenes = {};

        void Update() override;
    };
    Gleam_MakeSystem(HierarchyWindow_Scene)
}