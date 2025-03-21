#include "EditorUI.h"

#include "EditorUISerializer.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"

namespace Gleam
{
    void EditorUI::DrawSerializedContent(void* target, const std::type_index targetType)
    {
        EditorUISerializer serializer = {"TargetContent"};
        std::optional<std::reference_wrapper<const Type>> type = Type::GetType(targetType);
        if (type.has_value())
            type.value().get().Serialize(serializer, target);
        else
            serializer.FallbackTransfer(target, targetType);
    }
    void EditorUI::DrawEntityField(Entity& entity)
    {
        if (ImGui::Button(std::format("Entity:{}", static_cast<uint32_t>(entity)).c_str()))
        {
            static std::shared_ptr<Entity> inspecting = nullptr;
            inspecting = std::make_shared<Entity>(entity);
            GlobalInspectorWindow.SetTarget(inspecting);
        }
        if (ImGui::BeginDragDropSource())
        {
            static Entity draggingCache = Entity::Null;
            draggingCache = entity;
            ImGui::SetDragDropPayload(typeid(Entity).name(), &draggingCache, sizeof(draggingCache));
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (auto payload = ImGui::AcceptDragDropPayload(typeid(Entity).name()))
                entity = *static_cast<Entity*>(payload->Data);
            ImGui::EndDragDropTarget();
        }
    }
    void EditorUI::SetDragDropObject(const std::shared_ptr<void>& objectPtr, const std::type_index objectTypeIndex)
    {
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(
                std::to_string(objectTypeIndex.hash_code()).data(),
                &objectPtr, sizeof(std::shared_ptr<void>));
            ImGui::EndDragDropSource();
        }
    }
    std::shared_ptr<void> EditorUI::GetDragDropObject(const std::type_index objectType)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (auto payload = ImGui::AcceptDragDropPayload(std::to_string(objectType.hash_code()).data()))
                return *static_cast<std::shared_ptr<void>*>(payload->Data);
            ImGui::EndDragDropTarget();
        }
        return nullptr;
    }
}