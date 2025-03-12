#pragma once
#include "GleamUI/Runtime/UI.h"
#include "EditorTimeSystem.h"
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

#ifdef GleamAssetsRuntime
#include "GleamAssets/Runtime/Scene/Scene.h"
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
        inline static std::vector<Entity> removingEntities = {};
        inline static std::vector<System*> removingSystems = {};

        void Update() override
        {
            if (ImGui::Begin("HierarchyWindow"))
            {
                ImGui::SeparatorText("Statistics");
                ImGui::BulletText(std::format("IsPlaying:{}", Editor::IsPlaying()).c_str());
                ImGui::BulletText(std::format("NextEntity:{}", World::nextEntity).c_str());
                //帧率信息
                static float deltaTime = 0;
                deltaTime = std::lerp(deltaTime, EditorTimeSystem.GetDeltaTimeReal(), 0.3f);
                ImGui::BulletText(
                    "FrameRate:%5.1f ms/f (%5.1f FPS)",
                    deltaTime * 1000.0,
                    1.0 / deltaTime
                );

                ImGui::SeparatorText("World");
                DrawWorldUnfolding();
            }
            ImGui::End();

            for (auto system : removingSystems)
            {
#ifdef GleamAssetsRuntime
                auto optionalScene = Scene::GetScene(*system);
                if (optionalScene.has_value())
                    optionalScene->get().RemoveSystem(*system);
                else
#endif
                World::RemoveSystem(*system);
            }
            removingSystems.clear();
            for (auto entity : removingEntities)
            {
#ifdef GleamAssetsRuntime
                auto optionalScene = Scene::GetScene(entity);
                if (optionalScene.has_value())
                    optionalScene->get().RemoveEntity(entity);
#endif
                World::RemoveEntity(entity);
            }
            removingEntities.clear();
        }
    };
    Gleam_MakeGlobalSystem(HierarchyWindow)
}