#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/HierarchyWindow.h"

namespace Gleam
{
    class HierarchyWindow_Scene : public System
    {
    public:
        HierarchyWindow_Scene(): System(GlobalHierarchyWindow, OrderRelation::After)
        {
        }

        static void DrawScenePopup(Scene& scene);
        static void DrawScene(Scene& scene);
    private:
        inline static std::vector<Scene*> removingScenes = {};
        
        void Update() override;
    };
    Gleam_MakeGlobalSystem(HierarchyWindow_Scene)
}