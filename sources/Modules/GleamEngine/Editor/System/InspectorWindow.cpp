#include "InspectorWindow.h"

#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Editor/EditorUI/EditorUISerializer.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    bool& InspectorWindow::UseDebugGUI()
    {
        return useDebugGUI;
    }
    const InspectorWindow::CustomUI& InspectorWindow::GetCustomUI()
    {
        return inspectorGUIs;
    }
    void InspectorWindow::AddCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorUI)
    {
        inspectorGUIs.insert({typeIndex, drawInspectorUI});
    }
    void InspectorWindow::Show(const InspectorTarget& inspectorTarget)
    {
        static std::unique_ptr<InspectorWindow> copyWindowCache;

        copyWindowCache = std::make_unique<InspectorWindow>();
        copyWindowCache->SetTarget(inspectorTarget);
        World::AddSystem(*copyWindowCache);
    }

    const InspectorTarget& InspectorWindow::GetTarget() const
    {
        return inspectorTarget;
    }
    void InspectorWindow::SetTarget(const InspectorTarget& target)
    {
        this->inspectorTarget = target;
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
            if (ImGui::MenuItem("Clone"))
                Show(inspectorTarget);
            if (ImGui::MenuItem("Clear"))
                SetTarget(nullptr);
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::Checkbox("UseDebugGUI", &useDebugGUI);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        //绘制目标
        if (!inspectorTarget.objectPtr.expired())
        {
            auto [objectPtr, objectTypeIndex] = inspectorTarget;
            if (inspectorGUIs.contains(objectTypeIndex))
                inspectorGUIs[objectTypeIndex](objectPtr.lock().get());
            else
                EditorUI::DrawSerializedContent(objectPtr.lock().get(), objectTypeIndex);
        }
        else
            ImGui::Text("Target has expired");

        ImGui::End();
    }
}