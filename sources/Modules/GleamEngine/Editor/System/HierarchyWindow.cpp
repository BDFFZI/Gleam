#include "HierarchyWindow.h"

#include "InspectorWindow.h"
#include "GleamECS/Runtime/Scene.h"
#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Editor/Editor.h"
#include "GleamUI/Runtime/UI.h"
#include "GleamUtility/Runtime/Ranges.h"

namespace Gleam
{
    void HierarchyWindow::DrawEntity(const Entity entity)
    {
        EditorUI::EntityField(entity);
        DrawEntityPopup(entity);
    }
    void HierarchyWindow::DrawSystem(System& system)
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
            {ImGui::GetContentRegionAvail().x - ImGui::GetTextLineHeightWithSpacing() * 1.5f, 0} //按钮铺满当前行余下的所有空间
        ))
            GlobalInspectorWindow.SetTarget(InspectorTarget{system});
        DrawSystemPopup(system);

        //系统引用计数
        ImGui::SameLine();
        ImGui::Text("%i", World::systemUsageCount[&system]);

        if (systemGroup && collapsing)
        {
            ImGui::TreePush(systemGroup->GetName().c_str());
            DrawSubSystems(*systemGroup);
            ImGui::TreePop();
        }
    }
    void HierarchyWindow::DrawSubSystems(SystemGroup& systemGroup)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamGreen());
        for (const auto subSystem : systemGroup.subSystemStartQueue)
            DrawSystem(*subSystem);
        ImGui::PopStyleColor();

        ImGui::PushStyleColor(ImGuiCol_Header, ImGui::GetStyleColorVec4(ImGuiCol_Header) * float4::GleamRed());
        for (const auto subSystem : systemGroup.subSystemStopQueue)
            DrawSystem(*subSystem);
        ImGui::PopStyleColor();

        for (const auto subSystem : systemGroup.subSystemUpdateQueue)
            DrawSystem(*subSystem);
    }

    void HierarchyWindow::DrawSystemsPopup(const std::optional<std::reference_wrapper<Scene>> scene)
    {
        ImGuiID addSystemID = ImGui::GetID("AddSystem");
        if (ImGui::BeginPopup("AddSystem"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("##");
            if (ImGui::BeginListBox("##"))
            {
                for (System& system : System::GetAllSystems())
                {
                    if (filter.PassFilter(system.GetName().data()) && ImGui::Button(system.GetName().data()))
                    {
                        if (scene.has_value())
                            scene.value().get().AddSystem(system);
                        else
                            World::AddSystem(system);

                        ImGui::CloseCurrentPopup();
                        break;
                    }
                }

                ImGui::EndListBox();
            }
            ImGui::EndPopup();
        }

        std::string id = std::format("{}SystemsPopup", scene.has_value() ? scene->get().GetName() : "");
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("AddSystem"))
            {
                ImGui::CloseCurrentPopup();
                ImGui::OpenPopup(addSystemID);
            }

            ImGui::EndPopup();
        }
    }
    void HierarchyWindow::DrawEntitiesPopup(const std::optional<std::reference_wrapper<Scene>> scene)
    {
        std::string id = std::format("{}EntitiesPopup", scene.has_value() ? scene->get().GetName() : "");
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("AddEntity"))
            {
                if (scene.has_value())
                {
                    Entity entity = World::AddEntity();
                    scene->get().AddEntity(entity);
                }
                else
                    World::AddEntity();

                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    void HierarchyWindow::DrawSystemPopup(System& system)
    {
        std::string id = std::format("{}SystemPopup", system.GetName());
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("RemoveSystem"))
            {
                removingSystems.push_back(&system);
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
    void HierarchyWindow::DrawEntityPopup(Entity entity)
    {
        std::string id = std::format("{}EntityPopup", static_cast<uint32_t>(entity));
        if (ImGui::BeginPopupContextItem(id.data()))
        {
            if (ImGui::Button("RemoveEntity"))
            {
                removingEntities.push_back(entity);
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void HierarchyWindow::DrawWorld()
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
    void HierarchyWindow::DrawWorldUnfolding()
    {
        const bool systemsCollapsing = ImGui::CollapsingHeader("Systems");
        DrawSystemsPopup(std::nullopt);
        if (systemsCollapsing)
        {
            DrawSubSystems(World::GetSystems());
        }

        const bool entityCollapsing = ImGui::CollapsingHeader("Entities");
        DrawEntitiesPopup(std::nullopt);
        if (entityCollapsing)
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
                        DrawEntity(entity);
                    });

                    ImGui::TreePop();
                }
            }
        }
    }
    void HierarchyWindow::DrawScene(Scene& scene)
    {
        if (ImGui::CollapsingHeader(scene.GetName().data()))
        {
            ImGui::TreePush(scene.GetName().data());

            const bool systemsCollapsing = ImGui::CollapsingHeader("Systems");
            DrawSystemsPopup(scene);
            if (systemsCollapsing)
            {
                for (auto entity : scene.GetSystems())
                    DrawSystem(*entity);
            }

            const bool entityCollapsing = ImGui::CollapsingHeader("Entities");
            DrawEntitiesPopup(scene);
            if (entityCollapsing)
            {
                for (auto entity : scene.GetEntities())
                    DrawEntity(entity);
            }
            ImGui::TreePop();
        }
    }

    void HierarchyWindow::Update()
    {
        ImGui::Begin("HierarchyWindow");

        ImGui::SeparatorText("Statistics");
        ImGui::BulletText(std::format("IsPlaying:{}", Editor::IsPlaying()).c_str());
        ImGui::BulletText(std::format("NextEntity:{}", World::nextEntity).c_str());

        ImGui::SeparatorText("World");
        DrawWorldUnfolding();

        ImGui::SeparatorText("Scene");
        for (auto& scene : Scene::GetAllScenes() | UnwrapRef)
            DrawScene(scene);
        ImGui::End();

        for (auto system : removingSystems)
        {
            auto optionalScene = Scene::GetScene(*system);
            if (!optionalScene.has_value())
                World::RemoveSystem(*system);
            else
                optionalScene->get().RemoveSystem(*system);
        }
        removingSystems.clear();
        for (auto entity : removingEntities)
        {
            auto optionalScene = Scene::GetScene(entity);
            if (optionalScene.has_value())
                optionalScene->get().RemoveEntity(entity);
            World::RemoveEntity(entity);
        }
        removingEntities.clear();
    }
}