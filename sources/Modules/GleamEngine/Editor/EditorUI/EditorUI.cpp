#include "EditorUI.h"

#include "GleamECS/Runtime/Scene.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"

namespace Gleam
{
    void EditorUI::EntityField(Entity entity)
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