#pragma once
#include "GleamUI/Runtime/UI.h"
#include "EditorTimeSystem.h"
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

#ifdef GleamAssetsRuntime
#include "GleamECS/Runtime/Scene.h"
#endif

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
        friend void HierarchyWindow_FlushRemovingQueue();
        inline static std::vector<Entity> removingEntities = {};
        inline static std::vector<System*> removingSystems = {};

        void Update() override;
    };
    Gleam_MakeGlobalSystem(HierarchyWindow)

    /**
     * 有时资源不能立即回收（如渲染资源），因为被占用中，故在系统事件之外回收
     */
    inline void HierarchyWindow_FlushRemovingQueue()
    {
        for (auto system : HierarchyWindow::removingSystems)
        {
#ifdef GleamAssetsRuntime
            auto optionalScene = Scene::GetScene(*system);
            if (optionalScene.has_value())
                optionalScene->get().RemoveSystem(*system);
            else
#endif
            World::RemoveSystem(*system);
        }
        HierarchyWindow::removingSystems.clear();
        for (auto entity : HierarchyWindow::removingEntities)
        {
#ifdef GleamAssetsRuntime
            auto optionalScene = Scene::GetScene(entity);
            if (optionalScene.has_value())
                optionalScene->get().RemoveEntity(entity);
#endif
            World::RemoveEntity(entity);
        }
        HierarchyWindow::removingEntities.clear();
    }
}