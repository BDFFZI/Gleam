#include "HierarchyWindow.h"
#include "EditorUIUtility.h"
#include "InspectorWindow.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void HierarchyWindow::Update()
    {
        ImGui::Begin("HierarchyWindow");

        ImGui::SeparatorText("Statistics");
        ImGui::BulletText(std::format("TotalEntity:{}", World::nextEntity - 1).c_str());

        ImGui::SeparatorText("Details");
        if (ImGui::CollapsingHeader("System"))
        {
            EditorUIUtility::DrawSystemGroup(World::allSystems);
        }
        if (ImGui::CollapsingHeader("Archetype"))
        {
            for (auto& [archetype,heap] : World::entities)
            {
                if (ImGui::TreeNode(archetype->name))
                {
                    heap.ForeachElements([](std::byte* item)
                    {
                        Entity& entity = *reinterpret_cast<Entity*>(item);
                        EditorUIUtility::DrawEntityButton(entity);
                    });

                    ImGui::TreePop();
                }
            }
        }


        ImGui::End();
    }
}
