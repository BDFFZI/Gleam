#include "EditorUIUtility.h"
#include "LightUI/Runtime/UI.h"
#include "InspectorWindow.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    void EditorUIUtility::DrawEntityButton(Entity entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            InspectorWindow.target = entity;
        }
        // if(ImGui::BeginDragDropSource())
        // {
        //     ImGui::SetDragDropPayload("Dragging", &entity, sizeof(ObjectPtrBase));
        //     ImGui::EndDragDropSource();
        // }
    }
    void EditorUIUtility::DrawSystemGroup(SystemGroup* systemGroup)
    {
        if (ImGui::TreeNode(typeid(systemGroup).name()))
        {
            for (const auto subSystem : systemGroup->subSystemUpdateQueue)
            {
                if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                    DrawSystemGroup(subSystemGroup);
                else
                    ImGui::Text(typeid(*subSystem).name());
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
                        EditorUIUtility::DrawEntityButton(entity);
                    });

                    ImGui::TreePop();
                }
            }
        }
    }
}