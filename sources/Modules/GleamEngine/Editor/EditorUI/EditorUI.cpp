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
            inspecting = entity;
            GlobalInspectorWindow.SetTarget(InspectorTarget{inspecting});
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
    void EditorUI::SetDragDropObject(void* object, const Type& objectType)
    {
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(
                std::to_string(objectType.GetIndex().hash_code()).data(),
                static_cast<void**>(&object), sizeof(void*));
            ImGui::EndDragDropSource();
        }
    }
    void* EditorUI::GetDragDropObject(const Type& objectType)
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (auto payload = ImGui::AcceptDragDropPayload(std::to_string(objectType.GetIndex().hash_code()).data()))
                return *static_cast<void**>(payload->Data);
            ImGui::EndDragDropTarget();
        }
        return nullptr;
    }
}