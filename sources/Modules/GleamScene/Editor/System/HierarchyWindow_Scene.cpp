#include "HierarchyWindow_Scene.h"

#include <imgui.h>

#include "GleamECS/Runtime/Scene.h"
#include "GleamScene/Editor/EditorSceneManager.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    void HierarchyWindow_Scene::DrawScenePopup(Scene& scene)
    {
        if (ImGui::BeginPopupContextItem("ScenePopup"))
        {
            if (ImGui::MenuItem("Save"))
                EditorSceneManager::SaveScene(scene);
            if (ImGui::MenuItem("Close"))
                removingScenes.emplace_back(&scene);

            ImGui::EndPopup();
        }
    }
    void HierarchyWindow_Scene::DrawScene(Scene& scene)
    {
        const bool sceneCollapsing = ImGui::CollapsingHeader(scene.GetName().data());
        DrawScenePopup(scene);
        if (sceneCollapsing)
        {
            ImGui::TreePush(scene.GetName().data());

            const bool systemsCollapsing = ImGui::CollapsingHeader("Systems");
            HierarchyWindow::DrawSystemsPopup(scene);
            if (systemsCollapsing)
            {
                for (auto entity : scene.GetSystems())
                    HierarchyWindow::DrawSystem(*entity);
            }

            const bool entityCollapsing = ImGui::CollapsingHeader("Entities");
            HierarchyWindow::DrawEntitiesPopup(scene);
            if (entityCollapsing)
            {
                for (auto entity : scene.GetEntities())
                    HierarchyWindow::DrawEntity(entity);
            }
            ImGui::TreePop();
        }
    }
    void HierarchyWindow_Scene::Update()
    {
        if (ImGui::Begin("HierarchyWindow"))
        {
            ImGui::SeparatorText("Scene");
            for (auto& scene : Scene::GetAllScenes() | UnwrapRef)
                DrawScene(scene);
        }
        ImGui::End();

        for (auto scene : removingScenes)
            EditorSceneManager::CloseScene(*scene);
        removingScenes.clear();
    }
}