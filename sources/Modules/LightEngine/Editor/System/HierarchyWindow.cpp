#include "HierarchyWindow.h"

#include "LightEngine/Editor/EditorUI/EditorUI.h"
#include "LightECS/Runtime/World.h"
#include "LightEngine/Editor/Editor.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void HierarchyWindow::Update()
    {
        ImGui::Begin("HierarchyWindow");

        ImGui::SeparatorText("Statistics");
        ImGui::BulletText(std::format("IsPlaying:{}", Editor::IsPlaying()).c_str());
        ImGui::BulletText(std::format("TotalEntity:{}", World::nextEntity - 1).c_str());

        ImGui::SeparatorText("World");
        EditorUI::DrawWorldUnfolding();
        
        ImGui::SeparatorText("Scenes");
        // for (auto* scene : Editor::GetScenes())
        //     EditorUI::DrawScene(scene);

        ImGui::End();
    }
}