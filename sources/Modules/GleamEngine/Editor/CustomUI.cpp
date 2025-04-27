#include "CustomUI.h"

#include "EditorUI/EditorUI.h"
#include "EditorUI/EditorUISerializer.h"
#include "GleamUI/Runtime/UI.h"
#include "System/InspectorWindow.h"

namespace Gleam
{
    void DrawComponent(void* component, const Type& componentType, const InspectorWindow::CustomUI& componentGUI, bool* isUsing)
    {
        std::type_index componentTypeIndex = componentType.GetIndex();
        std::string_view componentName = componentType.GetName();

        ImGui::PushID(component);
        //绘制组件标题
        if (ImGui::CollapsingHeader(componentName.data(), isUsing, ImGuiTreeNodeFlags_DefaultOpen))
        {
            //绘制组件内容
            if (componentGUI.contains(componentTypeIndex))
                componentGUI.at(componentTypeIndex)(component);
            else //绘制默认组件内容
                EditorUI::DrawSerializedContent(component, componentTypeIndex);
        }
        ImGui::PopID();
    }
    void DrawEntity(const Entity entity, const InspectorWindow::CustomUI& componentGUI)
    {
        {
            EntityInfo entityInfo = World::GetEntityInfoAllocator().GetEntityInfo(entity);
            const Archetype& archetype = *entityInfo.archetype;
            //绘制实体编号
            ImGui::Text("Entity:%i", static_cast<int>(entity));
            ImGui::SameLine();
            ImGui::Text("Archetype:%s", archetype.GetName().data());
            //绘制组件
            for (int i = 0; i < archetype.GetComponentCount(); ++i)
            {
                const Type& componentType = archetype.GetComponentType(i);
                void* component = entityInfo.memoryAddress + archetype.GetComponentOffset(i);

                bool isUsing = true;
                DrawComponent(component, componentType, componentGUI, &isUsing);
                if (isUsing == false) //删除组件
                    World::RemoveComponentsAsync(entity, {componentType});
            }
        }

        //绘制操作
        ImGui::Separator();
        //添加组件
        const Type* selectedComponent = nullptr;
        ImGuiID selectComponentPopup = EditorUI::DrawSelectComponentPopup(selectedComponent);
        if (ImGui::Button("AddComponent", float2{ImGui::GetContentRegionAvail().x, 0}))
            ImGui::OpenPopup(selectComponentPopup);
        if (selectedComponent != nullptr)
            World::GetEntityAllocator().AddComponents(entity, {*selectedComponent});
        //移动实体
        const Archetype* selectedArchetype = nullptr;
        ImGuiID selectArchetypePopup = EditorUI::DrawSelectArchetypePopup(selectedArchetype);
        if (ImGui::Button("SetArchetype", float2{ImGui::GetContentRegionAvail().x, 0}))
            ImGui::OpenPopup(selectArchetypePopup);
        if (selectedArchetype != nullptr)
            World::MoveEntityAsync(entity, *selectedArchetype);
    }

    void InspectorWindowUI_Entity(const Entity entity)
    {
        if (World::GetEntityInfoAllocator().HasEntity(entity))
        {
            InspectorWindowUI_Entity_Target = entity;
            DrawEntity(
                entity,
                InspectorWindow::GetIsDebugGUI() ? InspectorWindow::CustomUI{} : InspectorWindow::GetCustomUI()
            );
        }
    }
    void InspectorWindowUI_LocalTransform(LocalTransform& localTransform)
    {
        EditorUISerializer serializer = {"LocalTransform"};
        serializer.TransferField("position", localTransform.position);
        float3 eulerAngles = localTransform.rotation.ToEulerAngles();
        serializer.TransferField("rotation", eulerAngles);
        localTransform.rotation = Quaternion::Euler(eulerAngles);
        serializer.TransferField("scale", localTransform.scale);
    }
}