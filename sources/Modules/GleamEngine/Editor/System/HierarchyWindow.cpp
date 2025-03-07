#include "HierarchyWindow.h"

#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Editor/Editor.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void HierarchyWindow::Update()
    {
        ImGui::Begin("HierarchyWindow");

        ImGui::SeparatorText("Statistics");
        ImGui::BulletText(std::format("IsPlaying:{}", Editor::IsPlaying()).c_str());
        ImGui::BulletText(std::format("TotalEntity:{}", World::nextEntity - 1).c_str());

        ImGui::SeparatorText("World");
        EditorUI::DrawWorldUnfolding();

        ImGui::End();
    }
}