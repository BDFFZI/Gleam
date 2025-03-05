#include "SceneWindow.h"

#include <imgui.h>

#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamScene/Runtime/Scene.h"

namespace Gleam
{
    void SceneWindow::Update()
    {
        ImGui::Begin("SceneWindow");

        for (auto& scene : Scene::allScenes | std::views::values)
        {
            if (ImGui::CollapsingHeader(scene.name.data()))
            {
                if (ImGui::CollapsingHeader("Entities"))
                {
                    for (auto& entity : scene.entities)
                        EditorUI::DrawEntity(entity);
                }
                if (ImGui::CollapsingHeader("Systems"))
                {
                    for (auto& system : scene.systems)
                        EditorUI::DrawSystem(*system);
                }
            }
        }

        ImGui::End();
    }
}