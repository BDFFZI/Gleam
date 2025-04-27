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
            GlobalInspectorWindow->SetMajorTarget(entity);
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

    ImGuiID EditorUI::DrawSelectSystemPopup(const SystemInfo*& outSystemInfo)
    {
        if (ImGui::BeginPopup("SelectSystem"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("##");
            if (ImGui::BeginListBox("##"))
            {
                for (SystemInfo& system : SystemInfoAllocator::GetAllSystemInfo())
                {
                    std::string_view name = system.type->GetName();
                    if (filter.PassFilter(name.data()) && ImGui::Button(name.data()))
                    {
                        outSystemInfo = &system;
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }

                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }
        return ImGui::GetID("SelectSystem");
    }
    ImGuiID EditorUI::DrawSelectComponentPopup(const Type*& outComponent)
    {
        if (ImGui::BeginPopup("SelectComponent"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("##");
            if (ImGui::BeginListBox("##"))
            {
                for (Type& type : Type::GetAllTypes())
                {
                    if (filter.PassFilter(type.GetName().data()) && ImGui::Button(type.GetName().data()))
                    {
                        outComponent = &type;
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }

                ImGui::EndListBox();
            }

            ImGui::EndPopup();
        }
        return ImGui::GetID("SelectComponent");
    }
    ImGuiID EditorUI::DrawSelectArchetypePopup(const Archetype*& outArchetype)
    {
        if (ImGui::BeginPopup("SelectArchetype"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("##");
            if (ImGui::BeginListBox("##"))
            {
                for (const Archetype& archetype : Archetype::GetAllArchetypes())
                {
                    if (filter.PassFilter(archetype.GetName().data()) && ImGui::Button(archetype.GetName().data()))
                    {
                        outArchetype = &archetype;
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }

                ImGui::EndListBox();
            }

            ImGui::EndPopup();
        }

        return ImGui::GetID("SelectArchetype");
    }
}