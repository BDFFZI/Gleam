#include "CustomUI.h"

#include "EditorUI/EditorUISerializer.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void DrawComponent(void* component, const Type& componentType, const CustomUI& componentGUI, bool* isUsing)
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
    void DrawEntity(const Entity entity, const CustomUI& componentGUI)
    {
        EntityInfo entityInfo = World::GetEntityInfo(entity);
        const Archetype& archetype = *entityInfo.archetype;
        //绘制实体编号
        ImGui::Text("Entity:%i", static_cast<int>(entity));
        ImGui::SameLine();
        ImGui::Text("Archetype:%s", archetype.GetName().data());
        //绘制组件
        for (int i = 0; i < archetype.GetComponentCount(); ++i)
        {
            const Type& componentType = archetype.GetComponentType(i);
            void* component = entityInfo.components + archetype.GetComponentOffset(i);

            bool isUsing = true;
            DrawComponent(component, componentType, componentGUI, &isUsing);
            if (isUsing == false) //删除组件
            {
                World::RemoveComponents(entity, {componentType});
                return;
            }
        }
        //添加组件
        ImGuiID addComponent = ImGui::GetID("AddComponent");
        if (ImGui::BeginPopup("AddComponent"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("##");
            if (ImGui::BeginListBox("##"))
            {
                for (Type& type : Type::GetAllTypes())
                {
                    if (type.GetParent() == SystemType)
                        continue;

                    if (filter.PassFilter(type.GetName().data()) && ImGui::Button(type.GetName().data()))
                    {
                        World::AddComponents(entity, {type});
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }

                ImGui::EndListBox();
            }

            ImGui::EndPopup();
        }

        ImGui::Separator();
        if (ImGui::Button("AddComponent", float2{ImGui::GetContentRegionAvail().x, 0}))
            ImGui::OpenPopup(addComponent);
    }

    void InspectorUI_Entity(const Entity entity)
    {
        if (World::HasEntity(entity))
        {
            InspectorUI_Entity_Target = entity;
            DrawEntity(
                entity,
                InspectorWindow::UseDebugGUI() ? CustomUI{} : InspectorWindow::GetCustomUI()
            );
        }
    }
    void InspectorUI_LocalTransform(LocalTransform& localTransform)
    {
        EditorUISerializer serializer = {"LocalTransform"};
        serializer.TransferField("position", localTransform.position);
        float3 eulerAngles = localTransform.rotation.ToEulerAngles();
        serializer.TransferField("rotation", eulerAngles);
        localTransform.rotation = Quaternion::Euler(eulerAngles);
        serializer.TransferField("scale", localTransform.scale);
    }
}