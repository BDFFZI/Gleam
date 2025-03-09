#include "EditorUI.h"

#include "EditorUISerializer.h"
#include "GleamECS/Runtime/Scene.h"
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
            serializer.Transfer(target, targetType);
    }
    void EditorUI::DrawEntityField(Entity entity)
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
}