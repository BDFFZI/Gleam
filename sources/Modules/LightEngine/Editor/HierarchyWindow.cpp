#include "HierarchyWindow.h"
#include "EditorUI/EditorUIUtility.h"
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
            EditorUIUtility::DrawSystemGroupUnfolding(World::GetMainSystem());
        if (ImGui::CollapsingHeader("MainScene"))
            EditorUIUtility::DrawSceneUnfolding(World::GetMainScene());


        ImGui::End();
    }
}