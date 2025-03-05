#include "EditorUI.h"

#include "EditorUISerializer.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"

namespace Gleam
{
    Entity EditorUI::GetEntityDrawing()
    {
        return drawing;
    }

    void EditorUI::DrawEntity(Entity entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            inspecting = entity;
            GlobalInspectorWindow.SetTarget(InspectorTarget{inspecting});
        }
        if (ImGui::BeginDragDropSource())
        {
            dragging = entity;
            ImGui::SetDragDropPayload(typeid(dragging).name(), &dragging, sizeof(dragging));
            ImGui::EndDragDropSource();
        }
    }
    void EditorUI::DrawSystem(System& system)
    {
        SystemGroup* systemGroup = dynamic_cast<SystemGroup*>(&system);

        //下拉框
        bool collapsing = ImGui::CollapsingHeader(
            std::format("##{}", system.GetName()).c_str(),
            (systemGroup == nullptr || systemGroup->subSystemUpdateQueue.empty()
                 ? ImGuiTreeNodeFlags_Leaf : 0) //无子系统时不显示箭头
            | ImGuiTreeNodeFlags_AllowItemOverlap //支持叠加按钮
        );
        //系统选中按钮
        ImGui::SameLine(); //放在下拉框旁边
        if (ImGui::Button(
            system.GetName().c_str(),
            {ImGui::GetContentRegionAvail().x - ImGui::GetTextLineHeightWithSpacing() * 1.5f, 0} //按钮铺满当前行余下的所有空间
        ))
            GlobalInspectorWindow.SetTarget(InspectorTarget{system});
        //系统引用计数
        ImGui::SameLine();
        ImGui::Text("%i", World::systemUsageCount[&system]);

        if (systemGroup && collapsing)
        {
            ImGui::TreePush(systemGroup->GetName().c_str());
            DrawSubSystems(*systemGroup);
            ImGui::TreePop();
        }
    }
    void EditorUI::DrawSubSystems(SystemGroup& systemGroup)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamGreen());
        for (const auto subSystem : systemGroup.subSystemStartQueue)
            DrawSystem(*subSystem);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamRed());
        for (const auto subSystem : systemGroup.subSystemStopQueue)
            DrawSystem(*subSystem);
        ImGui::PopStyleColor();

        for (const auto subSystem : systemGroup.subSystemUpdateQueue)
            DrawSystem(*subSystem);
    }

    void EditorUI::DrawWorld()
    {
        if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TreePush("World");
            DrawWorldUnfolding();
            ImGui::TreePop();
        }
        // if (ImGui::BeginDragDropTarget())
        // {
        //     if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(dragging).name()))
        //     {
        //         Entity entity = *static_cast<Entity*>(payload->Data);
        //         World::MoveEntity(entity, scene);
        //     }
        //     ImGui::EndDragDropTarget();
        // }
    }
    void EditorUI::DrawWorldUnfolding()
    {
        if (ImGui::CollapsingHeader("Systems"))
        {
            DrawSubSystems(World::GetSystems());
        }
        if (ImGui::CollapsingHeader("Entities"))
        {
            for (auto& [archetype,heap] : World::GetEntities())
            {
                if (heap.GetCount() == 0)
                    continue;

                if (ImGui::TreeNode(archetype->GetName().c_str()))
                {
                    heap.ForeachElements([](std::byte* item)
                    {
                        Entity& entity = *reinterpret_cast<Entity*>(item);
                        DrawEntity(entity);
                    });

                    ImGui::TreePop();
                }
            }
        }
    }

    void EditorUI::DrawDefaultContent(void* target, const std::type_index targetType)
    {
        std::optional<std::reference_wrapper<const Type>> type = Type::GetType(targetType);
        if (type.has_value())
        {
            EditorUISerializer serializer = {"TargetContent"};
            type.value().get().Serialize(serializer, target);
        }
    }
    void EditorUI::DrawEntityContent(const Entity entity, const CustomUI& componentGUI, const bool pure)
    {
        drawing = entity;

        EntityInfo entityInfo = World::GetEntityInfo(entity);
        const Archetype& archetype = *entityInfo.archetype;
        if (!pure)
        {
            //绘制实体编号
            ImGui::Text("Entity:%i", static_cast<int>(entity));
        }
        //绘制组件
        for (int i = 1; i < archetype.GetComponentCount(); ++i)
        {
            //统计实体信息
            const Type& componentType = archetype.GetComponentType(i);
            std::type_index componentTypeIndex = componentType.GetIndex();
            std::string_view componentName = componentType.GetName();
            int componentOffset = archetype.GetComponentOffset(i);
            void* component = entityInfo.components + componentOffset;
            //绘制
            ImGui::PushID(componentName.data());
            {
                if (!pure)
                {
                    //绘制组件标题
                    ImGui::SeparatorText(componentName.data());
                }

                //绘制组件内容
                if (componentGUI.contains(componentTypeIndex))
                    componentGUI.at(componentTypeIndex)(component);
                else if (!pure) //绘制默认组件内容
                    DrawDefaultContent(component, componentTypeIndex);
            }
            ImGui::PopID();
        }
    }
}