#include "HierarchyWindow.h"

#include "GleamUI/Runtime/UI.h"

#include "EditorTimeSystem.h"
#include "InspectorWindow.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    bool HierarchyWindow::DrawSystem(const ISystemEvent& system)
    {
        const SystemInfo& systemInfo = SystemInfoAllocator::GetSystemInfo(system);
        const ISystemGroup* systemGroup = dynamic_cast<const ISystemGroup*>(&system);

        //下拉框
        bool collapsing = ImGui::CollapsingHeader(
            std::format("##{}", typeid(system).name()).c_str(),
            (systemGroup == nullptr || systemGroup->GetSystems().empty()
                 ? ImGuiTreeNodeFlags_Leaf : 0) //无子系统时不显示箭头
            | ImGuiTreeNodeFlags_AllowItemOverlap //支持叠加按钮
        );
        //系统选中按钮
        ImGui::SameLine(); //放在下拉框旁边
        if (ImGui::Button(
            typeid(system).name(),
            {ImGui::GetContentRegionAvail().x - ImGui::GetTextLineHeightWithSpacing() * 1.5f, 0} //按钮铺满当前行余下的所有空间
        ))
        {
            GlobalInspectorWindow->SetMajorTarget(World::GetSystemAllocator().GetSystemPtr(systemInfo));
        }
        if (DrawSystemPopup(system) == false)
            return false;

        //系统引用计数
        ImGui::SameLine();
        ImGui::Text("%i", World::GetSystemAllocator().GetSystemUsageCount(SystemInfoAllocator::GetSystemInfo(system)));

        if (systemGroup && collapsing)
        {
            ImGui::TreePush(typeid(*systemGroup).name());
            DrawSubSystems(*systemGroup);
            ImGui::TreePop();
        }

        return true;
    }
    void HierarchyWindow::DrawSubSystems(const ISystemGroup& systemGroup)
    {
        for (const auto subSystem : systemGroup.GetSystems())
        {
            if (!DrawSystem(*subSystem))
                break;
        }
    }
    bool HierarchyWindow::DrawSystemPopup(const ISystemEvent& system)
    {
        bool result = true;

        std::string id = std::format("{}SystemPopup", typeid(system).name());
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("RemoveSystem"))
            {
                World::RemoveSceneSystem(SystemInfoAllocator::GetSystemInfo(system));
                ImGui::CloseCurrentPopup();
                result = false;
            }

            ImGui::EndPopup();
        }
        return result;
    }
    void HierarchyWindow::DrawSystemsPopup()
    {
        const SystemInfo* selectedSystemInfo = nullptr;
        ImGuiID selectSystemPopup = EditorUI::DrawSelectSystemPopup(selectedSystemInfo);

        if (ImGui::BeginPopupContextItem("SystemsPopup"))
        {
            if (ImGui::Button("AddSystem"))
            {
                ImGui::CloseCurrentPopup();
                ImGui::OpenPopup(selectSystemPopup);
            }

            ImGui::EndPopup();
        }

        if (selectedSystemInfo != nullptr)
            World::AddSceneSystem(*selectedSystemInfo);
    }

    bool HierarchyWindow::DrawEntity(Entity entity)
    {
        EditorUI::DrawEntityField(entity);
        return DrawEntityPopup(entity);
    }
    void HierarchyWindow::DrawEntitiesPopup()
    {
        if (ImGui::BeginPopupContextItem("EntitiesPopup"))
        {
            if (ImGui::Button("AddEntity"))
            {
                World::GetEntityAllocator().AddEntity();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    bool HierarchyWindow::DrawEntityPopup(Entity entity)
    {
        bool result = true;

        std::string id = std::format("{}EntityPopup", static_cast<uint32_t>(entity));
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("RemoveEntity"))
            {
                World::RemoveSceneEntityAsync(entity);
                ImGui::CloseCurrentPopup();
                result = false;
            }
            if (ImGui::Button("Clone"))
            {
                Entity newEntity = World::GetEntityAllocator().CloneEntity(entity);
                if (auto optionalScene = World::GetScene(entity); optionalScene.has_value())
                    optionalScene->get().AddEntity(newEntity);

                ImGui::CloseCurrentPopup();
                GlobalInspectorWindow->SetMajorTarget(newEntity);
            }

            ImGui::EndPopup();
        }
        return result;
    }

    void HierarchyWindow::DrawWorld()
    {
        if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TreePush("World");
            DrawWorldUnfolding();
            ImGui::TreePop();
        }
    }
    void HierarchyWindow::DrawWorldUnfolding()
    {
        const bool systemsCollapsing = ImGui::CollapsingHeader("Systems");
        DrawSystemsPopup();
        if (systemsCollapsing)
        {
            ImGui::PushID("Systems");
            DrawSubSystems(World::GetSystemAllocator().GetRootSystem());
            ImGui::PopID();
        }

        const bool entityCollapsing = ImGui::CollapsingHeader("Entities");
        DrawEntitiesPopup();
        if (entityCollapsing)
        {
            ImGui::PushID("Entities");
            for (auto& [archetype,heap] : World::GetEntityAllocator().GetEntityHeaps())
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
            ImGui::PopID();
        }

        if (ImGui::CollapsingHeader("Archetype"))
        {
            ImGui::PushID("Archetype");
            for (const Archetype& archetype : Archetype::GetAllArchetypes())
            {
                if (ImGui::Button(archetype.GetName().data()))
                    GlobalInspectorWindow->SetMajorTarget(InspectorTarget{const_cast<Archetype&>(archetype)});
            }
            ImGui::PopID();
        }
    }

    void HierarchyWindow::Update()
    {
        if (ImGui::Begin("HierarchyWindow"))
        {
            ImGui::SeparatorText("Statistics");
            ImGui::BulletText(std::format("IsPlaying:{}", Editor::GetIsPlaying()).c_str());
            ImGui::BulletText(std::format("NextEntity:{}", static_cast<uint32_t>(World::GetEntityInfoAllocator().GetNextEntity())).c_str());
            //帧率信息
            static float deltaTime = 0;
            deltaTime = std::lerp(deltaTime, GlobalEditorTimeSystem->GetDeltaTimeReal(), 0.3f);
            ImGui::BulletText(
                "FrameRate:%5.1f ms/f (%5.1f FPS)",
                deltaTime * 1000.0,
                1.0 / deltaTime
            );

            ImGui::SeparatorText("World");
            DrawWorldUnfolding();
        }
        ImGui::End();
    }
}