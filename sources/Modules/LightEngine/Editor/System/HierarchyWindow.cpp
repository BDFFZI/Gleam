#include "HierarchyWindow.h"
#include "LightEngine/Editor/EditorUI/EditorUI.h"
#include "LightECS/Runtime/World.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void HierarchyWindow::Update()
    {
        ImGui::Begin("HierarchyWindow");

        ImGui::SeparatorText("Statistics");
        ImGui::BulletText(std::format("TotalEntity:{}", World::nextEntity - 1).c_str());

        ImGui::SeparatorText("Details");
        if (ImGui::CollapsingHeader("MainSystem"))
        {
            ImGui::TreePush("MainSystem");
            EditorUI::DrawSystemGroupContent(World::GetMainSystem());
            ImGui::TreePop();
        }
        if (ImGui::CollapsingHeader("MainScene"))
        {
            ImGui::TreePush("MainScene");
            EditorUI::DrawSceneUnfolding(World::GetMainScene());
            ImGui::TreePop();
        }


        ImGui::End();
    }
}