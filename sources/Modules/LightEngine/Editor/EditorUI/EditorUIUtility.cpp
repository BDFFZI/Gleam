#include "EditorUIUtility.h"

#include "EditorUISerializer.h"
#include "LightUI/Runtime/UI.h"
#include "LightECS/Runtime/World.h"
#include "LightEngine/Editor/InspectorWindow.h"

namespace Light
{
    Entity dragging;
    void EditorUIUtility::DrawEntityButton(Entity entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            InspectorWindow->SetTarget(reinterpret_cast<Entity*>(World::GetEntityInfo(entity).components));
        }
        if (ImGui::BeginDragDropSource())
        {
            dragging = entity;
            ImGui::SetDragDropPayload(typeid(dragging).name(), &dragging, sizeof(dragging));
            ImGui::EndDragDropSource();
        }
    }
    void EditorUIUtility::DrawEntity(Entity entity)
    {
        EntityInfo entityInfo = World::GetEntityInfo(entity);
        const Archetype& archetype = *entityInfo.archetype;
        //绘制实体信息
        ImGui::Text("Entity:%i", static_cast<int>(entity));
        //绘制组件
        for (int i = 1; i < archetype.GetComponentCount(); ++i)
        {
            const ComponentInfo& componentInfo = archetype.GetComponentInfo(i);

            std::type_index componentType = componentInfo.type;
            const char* componentName = componentInfo.type.name();
            int componentOffset = archetype.GetComponentOffset(i);
            void* component = entityInfo.components + componentOffset;
            //绘制组件标题
            ImGui::SeparatorText(componentName);
            ImGui::PushID(componentName);
            //绘制组件内容
            Type* type = Type::GetType(componentType);
            if (type != nullptr)
            {
                EditorUISerializer editorUiSerializer;
                type->serialize(editorUiSerializer, component);
            }
            ImGui::PopID();
        }
    }
    void EditorUIUtility::DrawSystemGroup(SystemGroup* systemGroup)
    {
        if (ImGui::TreeNode(systemGroup->GetName().c_str()))
        {
            DrawSystemGroupUnfolding(systemGroup);
            ImGui::TreePop();
        }
    }
    void EditorUIUtility::DrawSystemGroupUnfolding(SystemGroup* systemGroup)
    {
        for (const auto subSystem : systemGroup->subSystemUpdateQueue)
        {
            if (SystemGroup* subSystemGroup = dynamic_cast<SystemGroup*>(subSystem))
                DrawSystemGroup(subSystemGroup);
            else
                ImGui::Text(subSystem->GetName().c_str());
        }
    }
    void EditorUIUtility::DrawScene(Scene* scene)
    {
        if (ImGui::CollapsingHeader(scene->GetName().c_str()))
        {
            DrawSceneUnfolding(scene);
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(typeid(dragging).name()))
            {
                Entity entity = *static_cast<Entity*>(payload->Data);
                Scene::MoveEntity(entity, scene);
            }
            ImGui::EndDragDropTarget();
        }
    }
    void EditorUIUtility::DrawSceneUnfolding(Scene* scene)
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
}