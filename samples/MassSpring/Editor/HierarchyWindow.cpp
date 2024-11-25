#include "HierarchyWindow.h"

#include <imgui.h>

#include "InspectorWindow.h"
#include "LightECS/Runtime/World.h"

void HierarchyWindow::Update()
{
    ImGui::Begin("HierarchyWindow");

    ImGui::SeparatorText("Statistics");
    ImGui::BulletText(std::format("TotalEntity:{}", World::nextEntity - 1).c_str());

    ImGui::SeparatorText("Details");
    if (ImGui::CollapsingHeader("System"))
    {
        if (ImGui::TreeNode("SystemStartQueue"))
        {
            for (auto& system : World::systemStartQueue)
                ImGui::Text(system.type.name());

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("SystemStopQueue"))
        {
            for (auto& system : World::systemStopQueue)
                ImGui::Text(system.type.name());

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("SystemUpdateQueue"))
        {
            for (auto& system : World::systemUpdateQueue)
                ImGui::Text(system.type.name());

            ImGui::TreePop();
        }
    }
    if (ImGui::CollapsingHeader("Archetype"))
    {
        for (auto& [archetype,heap] : World::entities)
        {
            if (ImGui::TreeNode(archetype->name))
            {
                heap.ForeachElements([](std::byte* item)
                {
                    Entity entity = *reinterpret_cast<Entity*>(item);
                    if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
                    {
                        InspectorWindow::inspectingTarget = item;
                        InspectorWindow::inspectingMethod = InspectorWindow::InspectEntity;
                    }
                });

                ImGui::TreePop();
            }
        }
    }


    ImGui::End();
}
