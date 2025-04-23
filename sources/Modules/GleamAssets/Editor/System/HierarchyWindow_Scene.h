#pragma once
#include "GleamEngine/Editor/System/HierarchyWindow.h"

namespace Gleam
{
    class HierarchyWindow_Scene : public RelativeSystem<HierarchyWindow, SystemRelation::After>
    {
    public:
        static bool DrawSceneSystemInfoPopup(Scene& scene, const SystemInfo& systemInfo);
        static void DrawSceneSystemsPopup(Scene& scene);
        static void DrawSceneEntitiesPopup(Scene& scene);

        static void DrawScene(Scene& scene);
        static void DrawScenePopup(Scene& scene);

    private:
        void Update() override;
    };
    Gleam_MakeEditorSystem(HierarchyWindow_Scene)
}