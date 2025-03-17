#include "HierarchyWindow_Scene.h"

#include <imgui.h>

#include "GleamAssets/Editor/EditorSceneManager.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    void HierarchyWindow_Scene::DrawScene(Scene& scene)
    {
        const bool sceneCollapsing = ImGui::CollapsingHeader(scene.GetName().data());
        DrawScenePopup(scene);
        if (sceneCollapsing)
        {
            ImGui::TreePush(scene.GetName().data());

            const bool systemsCollapsing = ImGui::CollapsingHeader("Systems");
            DrawSceneSystemsPopup(scene);
            if (systemsCollapsing)
            {
                for (auto entity : scene.GetSystems())
                    HierarchyWindow::DrawSystem(*entity);
            }

            const bool entityCollapsing = ImGui::CollapsingHeader("Entities");
            DrawSceneEntitiesPopup(scene);
            if (entityCollapsing)
            {
                for (auto entity : scene.GetEntities())
                    HierarchyWindow::DrawEntity(entity);
            }
            ImGui::TreePop();
        }
    }
    void HierarchyWindow_Scene::DrawScenePopup(Scene& scene)
    {
        std::string id = std::format("{}ScenePopup", scene.GetName());
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::MenuItem("Save"))
                EditorSceneManager::SaveScene(scene);
            if (ImGui::MenuItem("Close"))
                removingScenes.emplace_back(&scene);

            ImGui::EndPopup();
        }
    }
    void HierarchyWindow_Scene::DrawSceneSystemsPopup(Scene& scene)
    {
        ImGuiID addSystemID = ImGui::GetID("AddSystem");
        if (ImGui::BeginPopup("AddSystem"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("##");
            if (ImGui::BeginListBox("##"))
            {
                for (System& system : System::GetAllGlobalSystems())
                {
                    if (filter.PassFilter(system.GetName().data()) && ImGui::Button(system.GetName().data()))
                    {
                        scene.AddSystem(system);
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }

                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupContextItem("SystemsPopup"))
        {
            if (ImGui::Button("AddSystem"))
            {
                ImGui::CloseCurrentPopup();
                ImGui::OpenPopup(addSystemID);
            }

            ImGui::EndPopup();
        }
    }
    void HierarchyWindow_Scene::DrawSceneEntitiesPopup(Scene& scene)
    {
        if (ImGui::BeginPopupContextItem("EntitiesPopup"))
        {
            if (ImGui::Button("AddEntity"))
            {
                scene.AddEntity(World::AddEntity());
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
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