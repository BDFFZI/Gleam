#include "InspectorWindow.h"

#include "GleamEngine/Editor/EditorUI/EditorUISerializer.h"
#include "GleamECS/Runtime/World.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
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
        if (this != &Gleam::InspectorWindow)
            delete this;
    }
    void InspectorWindow::Update()
    {
        if (this == &Gleam::InspectorWindow)
            ImGui::Begin("InspectorWindow", nullptr, ImGuiWindowFlags_MenuBar);
        else
        {
            //非默认检视窗口，支持多窗口和关闭功能
            bool isOpen = true;
            ImGui::Begin(
                std::format("InspectorWindow##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                this == &Gleam::InspectorWindow ? nullptr : &isOpen
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
            {
                EditorUISerializer serializer = {"InspectionTarget"};
                Type& type = Type::GetType(typeIndex);
                if (type.GetSerialize()) //序列化每个元素
                    type.GetSerialize()(serializer, data);
                else //未知类型，当成字段整体传输给序列化器判断
                    serializer.Transfer(data, typeIndex);
            }
        }

        ImGui::End();
    }
}