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
    void EditorUI::DrawEntityButton(Entity entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            InspectorWindow.SetTarget(entity);
        }
        if (ImGui::BeginDragDropSource())
        {
            dragging = entity;
            ImGui::SetDragDropPayload(typeid(dragging).name(), &dragging, sizeof(dragging));
            ImGui::EndDragDropSource();
        }
    }
    void EditorUI::DrawEntity(const Entity entity, const CustomUI& componentGUI)
    {
        drawing = entity;

        EntityInfo entityInfo = World::GetEntityInfo(entity);
        const Archetype& archetype = *entityInfo.archetype;
        //绘制实体信息
        ImGui::Text("Entity:%i", static_cast<int>(entity));
        //绘制组件
        for (int i = 1; i < archetype.GetComponentCount(); ++i)
        {
            //统计实体信息
            const ComponentInfo& componentInfo = archetype.GetComponentInfo(i);
            std::type_index componentType = componentInfo.type;
            const char* componentName = componentInfo.type.name();
            int componentOffset = archetype.GetComponentOffset(i);
            void* component = entityInfo.components + componentOffset;
            //绘制
            ImGui::PushID(componentName);
            {
                //绘制组件标题
                ImGui::SeparatorText(componentName);
                //绘制组件内容
                if (componentGUI.contains(componentType))
                    componentGUI.at(componentType)(component);
                else
                {
                    Type& type = Type::GetType(componentType);
                    if (type.Serialize())
                    {
                        EditorUISerializer editorUiSerializer = {componentName};
                        type.Serialize()(editorUiSerializer, component);
                    }
                }
            }
            ImGui::PopID();
        }
    }
    void EditorUI::DrawEntityPure(const Entity entity, const CustomUI& componentGUI)
    {
        drawing = entity;

        EntityInfo entityInfo = World::GetEntityInfo(entity);
        const Archetype& archetype = *entityInfo.archetype;
        //绘制组件
        for (int i = 1; i < archetype.GetComponentCount(); ++i)
        {
            //统计实体信息
            const ComponentInfo& componentInfo = archetype.GetComponentInfo(i);
            std::type_index componentType = componentInfo.type;
            const char* componentName = componentInfo.type.name();
            int componentOffset = archetype.GetComponentOffset(i);
            void* component = entityInfo.components + componentOffset;
            //绘制
            ImGui::PushID(componentName);
            {
                if (componentGUI.contains(componentType))
                    componentGUI.at(componentType)(component);
            }
            ImGui::PopID();
        }
    }
    void EditorUI::DrawSystemGroup(SystemGroup& systemGroup)
    {
        //绘制内部子系统
        if (DrawSystem(systemGroup))
        {
            ImGui::TreePush(systemGroup.GetName().c_str());
            DrawSystemGroupContent(systemGroup);
            ImGui::TreePop();
        }
    }
    bool EditorUI::DrawSystem(System& system)
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
            {ImGui::GetContentRegionAvail().x, 0} //按钮铺满当前行余下的所有空间
        ))
            InspectorWindow.SetTarget(&system);

        return collapsing;
    }
    void EditorUI::DrawSystemGroupContent(SystemGroup& systemGroup)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamGreen());
        for (const auto subSystem : systemGroup.subSystemStartQueue)
        {
            if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                DrawSystemGroup(*subSystemGroup);
            else
                DrawSystem(*subSystem);
        }
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamRed());
        for (const auto subSystem : systemGroup.subSystemStopQueue)
        {
            if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                DrawSystemGroup(*subSystemGroup);
            else
                DrawSystem(*subSystem);
        }
        ImGui::PopStyleColor();

        for (const auto subSystem : systemGroup.subSystemUpdateQueue)
        {
            if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                DrawSystemGroup(*subSystemGroup);
            else
                DrawSystem(*subSystem);
        }
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
            DrawSystemGroupContent(World::GetSystems());
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
                        DrawEntityButton(entity);
                    });

                    ImGui::TreePop();
                }
            }
        }
    }
    void EditorUI::DrawDefaultInspectorUI(void* target, const std::type_index targetType)
    {
        EditorUISerializer serializer = {"InspectionTarget"};
        Type& type = Type::GetType(targetType);
        if (type.Serialize()) //序列化每个元素
            type.Serialize()(serializer, target);
        else //未知类型，当成字段整体传输给序列化器判断
            serializer.Transfer(target, targetType);
    }
}