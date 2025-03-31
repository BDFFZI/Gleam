#include "HierarchyWindow.h"

#include "EditorTimeSystem.h"
#include "InspectorWindow.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Editor/Editor.h"

namespace Gleam
{
    bool HierarchyWindow::DrawEntity(Entity entity)
    {
        EditorUI::DrawEntityField(entity);
        return DrawEntityPopup(entity);
    }
    bool HierarchyWindow::DrawSystem(System& system)
    {
        SystemGroup* systemGroup = dynamic_cast<SystemGroup*>(&system);

        //下拉框
        bool collapsing = ImGui::CollapsingHeader(
            std::format("##{}", system.GetName()).c_str(),
            (systemGroup == nullptr || systemGroup->updatingSystems.empty()
                 ? ImGuiTreeNodeFlags_Leaf : 0) //无子系统时不显示箭头
            | ImGuiTreeNodeFlags_AllowItemOverlap //支持叠加按钮
        );
        //系统选中按钮
        ImGui::SameLine(); //放在下拉框旁边
        if (ImGui::Button(
            system.GetName().c_str(),
            {ImGui::GetContentRegionAvail().x - ImGui::GetTextLineHeightWithSpacing() * 1.5f, 0} //按钮铺满当前行余下的所有空间
        ))
        {
            GlobalInspectorWindow.SetTarget(InspectorTarget{system});
        }
        if (DrawSystemPopup(system) == false)
            return false;

        //系统引用计数
        ImGui::SameLine();
        ImGui::Text("%i", World::systemUsageCount[&system]);

        if (systemGroup && collapsing)
        {
            ImGui::TreePush(systemGroup->GetName().c_str());
            DrawSubSystems(*systemGroup);
            ImGui::TreePop();
        }

        return true;
    }
    void HierarchyWindow::DrawSubSystems(SystemGroup& systemGroup)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamGreen());
        for (const auto subSystem : systemGroup.addingSystems)
            DrawSystem(*subSystem);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamRed());
        for (const auto subSystem : systemGroup.removingSystems)
            DrawSystem(*subSystem);
        ImGui::PopStyleColor();

        for (const auto subSystem : systemGroup.updatingSystems)
            DrawSystem(*subSystem);
    }

    void HierarchyWindow::DrawSystemsPopup()
    {
        ImGuiID addSystemID = ImGui::GetID("AddSystem");
        if (ImGui::BeginPopup("AddSystem"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("##");
            if (ImGui::BeginListBox("##"))
            {
                for (System& system : System::GetAllGlobalSystems())
                {
                    if (filter.PassFilter(system.GetName().data()) && ImGui::Button(system.GetName().data()))
                    {
                        World::AddSystem(system);
                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }

                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginPopupContextItem("SystemsPopup"))
        {
            if (ImGui::Button("AddSystem"))
            {
                ImGui::CloseCurrentPopup();
                ImGui::OpenPopup(addSystemID);
            }

            ImGui::EndPopup();
        }
    }
    void HierarchyWindow::DrawEntitiesPopup()
    {
        if (ImGui::BeginPopupContextItem("EntitiesPopup"))
        {
            if (ImGui::Button("AddEntity"))
            {
                World::AddEntity();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    bool HierarchyWindow::DrawSystemPopup(System& system)
    {
        bool result = true;

        std::string id = std::format("{}SystemPopup", system.GetName());
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("RemoveSystem"))
            {
                World::RemoveSystem(system);
                ImGui::CloseCurrentPopup();
                result = false;
            }

            ImGui::EndPopup();
        }
        return result;
    }
    bool HierarchyWindow::DrawEntityPopup(Entity entity)
    {
        bool result = true;

        std::string id = std::format("{}EntityPopup", static_cast<uint32_t>(entity));
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("RemoveEntity"))
            {
                World::RemoveEntityAsync(entity);
                ImGui::CloseCurrentPopup();
                result = false;
            }
            if (ImGui::Button("Clone"))
            {
                Entity newEntity = World::CloneEntity(entity);
                ImGui::CloseCurrentPopup();
                GlobalInspectorWindow.SetTarget(newEntity);
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
            DrawSubSystems(World::GetRootSystemGroup());
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
                    GlobalInspectorWindow.SetTarget(InspectorTarget{const_cast<Archetype&>(archetype)});
            }
            ImGui::PopID();
        }
    }

    void HierarchyWindow::Update()
    {
        if (ImGui::Begin("HierarchyWindow"))
        {
            ImGui::SeparatorText("Statistics");
            ImGui::BulletText(std::format("IsPlaying:{}", Editor::IsPlaying()).c_str());
            ImGui::BulletText(std::format("NextEntity:{}", World::entityInfoAllocator.nextEntity).c_str());
            //帧率信息
            static float deltaTime = 0;
            deltaTime = std::lerp(deltaTime, EditorTimeSystem.GetDeltaTimeReal(), 0.3f);
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