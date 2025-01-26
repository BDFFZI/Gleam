#include "EditorUIUtility.h"
#include "LightUI/Runtime/UI.h"
#include "InspectorWindow.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    Entity dragging;
    void EditorUIUtility::DrawEntityButton(Entity entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            InspectorWindow->target = entity;
        }
        if (ImGui::BeginDragDropSource())
        {
            dragging = entity;
            ImGui::SetDragDropPayload(typeid(dragging).name(), &dragging, sizeof(dragging));
            ImGui::EndDragDropSource();
        }
    }
    void EditorUIUtility::DrawSystemGroup(SystemGroup* systemGroup)
    {
        if (ImGui::TreeNode(systemGroup->GetName().c_str()))
        {
            for (const auto subSystem : systemGroup->subSystemUpdateQueue)
            {
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    DrawSystemGroup(subSystemGroup);
                else
                    ImGui::Text(subSystem->GetName().c_str());
            }

            ImGui::TreePop();
        }
    }
    void EditorUIUtility::DrawScene(Scene* scene)
    {
        if (ImGui::CollapsingHeader(scene->GetName().c_str()))
        {
            for (auto& [archetype,heap] : scene->GetAllEntities())
            {
                if (heap.GetCount() == 0)
                    continue;

                if (ImGui::TreeNode(archetype->GetName().c_str()))
                {
                    heap.ForeachElements([](std::byte* item)
                    {
                        Entity& entity = *reinterpret_cast<Entity*>(item);
                        DrawEntityButton(entity);
                    });

                    ImGui::TreePop();
                }
            }
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(dragging).name()))
            {
                Entity entity = *static_cast<Entity*>(payload->Data);
                World::MoveEntity(entity, scene);
            }
            ImGui::EndDragDropTarget();
        }
    }
}