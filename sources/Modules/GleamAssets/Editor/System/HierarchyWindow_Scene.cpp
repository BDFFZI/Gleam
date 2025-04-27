#include "HierarchyWindow_Scene.h"

#include <imgui.h>

#include "GleamAssets/Editor/EditorSceneManager.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    bool HierarchyWindow_Scene::DrawSceneSystemInfoPopup(Scene& scene, const SystemInfo& systemInfo)
    {
        bool result = true;

        std::string id = std::format("{}SystemPopup", systemInfo.type->GetName());
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("RemoveSystem"))
            {
                scene.RemoveSystem(systemInfo);
                ImGui::CloseCurrentPopup();
                result = false;
            }

            ImGui::EndPopup();
        }

        return result;
    }
    void HierarchyWindow_Scene::DrawSceneSystemsPopup(Scene& scene)
    {
        const SystemInfo* selectedSystemInfo = nullptr;
        ImGuiID selectSystemPopup = EditorUI::DrawSelectSystemPopup(selectedSystemInfo);

        if (ImGui::BeginPopupContextItem("SystemsPopup"))
        {
            if (ImGui::Button("AddSystem"))
            {
                ImGui::CloseCurrentPopup();
                ImGui::OpenPopup(selectSystemPopup);
            }

            ImGui::EndPopup();
        }

        if (selectedSystemInfo != nullptr)
            scene.AddSystem(*selectedSystemInfo);
    }

    void HierarchyWindow_Scene::DrawSceneEntitiesPopup(Scene& scene)
    {
        if (ImGui::BeginPopupContextItem("EntitiesPopup"))
        {
            if (ImGui::Button("AddEntity"))
            {
                scene.AddEntity(World::GetEntityAllocator().AddEntity());
                ImGui::CloseCurrentPopup();
            }

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
            DrawSceneSystemsPopup(scene);
            if (systemsCollapsing)
            {
                for (auto systemInfo : scene.GetSystems())
                {
                    IOrderedSystemEvent* system = World::GetSystemAllocator().TryGetSystem(*systemInfo);
                    if (system != nullptr)
                    {
                        if (HierarchyWindow::DrawSystem(*system) == false)
                            break;
                    }
                    else
                    {
                        ImGui::Button(systemInfo->type->GetName().data());
                        if (!DrawSceneSystemInfoPopup(scene, *systemInfo))
                            break;
                    }
                }
            }

            const bool entityCollapsing = ImGui::CollapsingHeader("Entities");
            DrawSceneEntitiesPopup(scene);
            if (entityCollapsing)
            {
                for (auto entity : scene.GetEntities())
                {
                    if (HierarchyWindow::DrawEntity(entity) == false)
                        break;
                }
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
                EditorSceneManager::CloseScene(scene);

            ImGui::EndPopup();
        }
    }

    void HierarchyWindow_Scene::Update()
    {
        if (ImGui::Begin("HierarchyWindow"))
        {
            ImGui::SeparatorText("Scene");
            for (auto& scene : World::GetAllScenes() | UnwrapRef)
                DrawScene(scene);
        }
        ImGui::End();
    }
}