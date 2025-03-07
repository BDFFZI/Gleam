#include "InspectorWindow.h"

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Editor/EditorUI/EditorUISerializer.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    Entity InspectorWindow::GetEntityDrawing()
    {
        return drawing;
    }
    void InspectorWindow::DrawDefaultContent(void* target, const std::type_index targetType)
    {
        std::optional<std::reference_wrapper<const Type>> type = Type::GetType(targetType);
        if (type.has_value())
        {
            EditorUISerializer serializer = {"TargetContent"};
            type.value().get().Serialize(serializer, target);
        }
    }
    void InspectorWindow::DrawEntityContent(const Entity entity, const CustomUI& componentGUI, const bool pure)
    {
        drawing = entity;

        EntityInfo entityInfo = World::GetEntityInfo(entity);
        const Archetype& archetype = *entityInfo.archetype;
        if (!pure)
        {
            //绘制实体编号
            ImGui::Text("Entity:%i", static_cast<int>(entity));
        }
        //绘制组件
        for (int i = 1; i < archetype.GetComponentCount(); ++i) //第一个组件总是Entity，排除
        {
            //统计实体信息
            const Type& componentType = archetype.GetComponentType(i);
            std::type_index componentTypeIndex = componentType.GetIndex();
            std::string_view componentName = componentType.GetName();
            int componentOffset = archetype.GetComponentOffset(i);
            void* component = entityInfo.components + componentOffset;
            //绘制
            ImGui::PushID(std::format("component_{}", i).data());
            {
                if (!pure)
                {
                    //绘制组件标题
                    ImGui::SeparatorText(componentName.data());
                }

                //绘制组件内容
                if (componentGUI.contains(componentTypeIndex))
                    componentGUI.at(componentTypeIndex)(component);
                else if (!pure) //绘制默认组件内容
                    DrawDefaultContent(component, componentTypeIndex);
            }
            ImGui::PopID();
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

    bool& InspectorWindow::UseDebugGUI()
    {
        return useDebugGUI;
    }
    const CustomUI& InspectorWindow::GetCustomUI()
    {
        return inspectorGUIs;
    }
    void InspectorWindow::AddCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorUI)
    {
        inspectorGUIs.insert({typeIndex, drawInspectorUI});
    }
    void InspectorWindow::Show(InspectorTarget inspectorTarget)
    {
        InspectorWindow* inspectorWindow = new InspectorWindow();
        inspectorWindow->SetTarget(inspectorTarget);
        World::AddSystem(*inspectorWindow);
    }

    const std::optional<InspectorTarget>& InspectorWindow::GetTarget() const
    {
        return target;
    }
    void InspectorWindow::SetTarget(const std::optional<InspectorTarget>& target)
    {
        this->target = target;
    }


    void InspectorWindow::Stop()
    {
        if (this != &GlobalInspectorWindow)
            delete this;
    }
    void InspectorWindow::Update()
    {
        if (this == &GlobalInspectorWindow)
            ImGui::Begin("InspectorWindow", nullptr, ImGuiWindowFlags_MenuBar);
        else
        {
            //非默认检视窗口，支持多窗口和关闭功能
            bool isOpen = true;
            ImGui::Begin(
                std::format("InspectorWindow##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                this == &GlobalInspectorWindow ? nullptr : &isOpen
            );
            if (isOpen == false)
                World::RemoveSystem(*this);
        }

        //绘制菜单项
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::MenuItem("Clone") && target.has_value())
                Show(target.value());
            if (ImGui::MenuItem("Clear"))
                SetTarget(std::nullopt);
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::Checkbox("UseDebugGUI", &useDebugGUI);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        //绘制目标
        if (target.has_value())
        {
            auto [data, typeIndex] = target.value();
            if (inspectorGUIs.contains(typeIndex))
                inspectorGUIs[typeIndex](data);
            else
                DrawDefaultContent(data, typeIndex);
        }

        ImGui::End();
    }
}