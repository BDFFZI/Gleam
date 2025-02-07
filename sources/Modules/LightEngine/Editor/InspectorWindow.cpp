#include "InspectorWindow.h"

#include "EditorUI/EditorUISerializer.h"
#include "LightECS/Runtime/World.h"
#include "LightReflection/Runtime/Type.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    bool& InspectorWindow::UseDebugGUI()
    {
        return useDebugGUI;
    }
    const CustomGUI& InspectorWindow::GetCustomGUI()
    {
        return inspectorGUIs;
    }
    void InspectorWindow::AddCustomGUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorGUI)
    {
        inspectorGUIs.insert({typeIndex, drawInspectorGUI});
    }
    void InspectorWindow::Show(const Entity target)
    {
        InspectorWindow* inspectorWindow = new InspectorWindow();
        inspectorWindow->target = World::GetEntityInfo(target).components;
        inspectorWindow->targetType = typeid(Entity);
        World::AddSystem(inspectorWindow);
    }
    void InspectorWindow::Show(void* target, const std::type_index targetType)
    {
        InspectorWindow* inspectorWindow = new InspectorWindow();
        inspectorWindow->target = target;
        inspectorWindow->targetType = targetType;
        World::AddSystem(inspectorWindow);
    }
    void* InspectorWindow::GetTarget() const
    {
        return target;
    }
    std::type_index InspectorWindow::GetTargetType() const
    {
        return targetType;
    }
    void InspectorWindow::Stop()
    {
        if (this != Light::InspectorWindow)
            delete this;
    }
    void InspectorWindow::Update()
    {
        if (this == Light::InspectorWindow)
            ImGui::Begin("InspectorWindow", nullptr, ImGuiWindowFlags_MenuBar);
        else
        {
            //非默认检视窗口，支持多窗口和关闭功能
            bool isOpen = true;
            ImGui::Begin(
                std::format("InspectorWindow##{}", reinterpret_cast<uintptr_t>(target)).c_str(),
                this == Light::InspectorWindow ? nullptr : &isOpen
            );
            if (isOpen == false)
                World::RemoveSystem(this);
        }

        //绘制菜单项
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::MenuItem("Clone"))
                Show(target, targetType);
            if (ImGui::BeginMenu("Debug"))
            {
                ImGui::Checkbox("UseDebugGUI", &useDebugGUI);
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
        //绘制目标
        if (inspectorGUIs.contains(targetType))
            inspectorGUIs[targetType](target);
        else
        {
            EditorUISerializer serializer = {};
            serializer.Transfer(target, targetType);
        }

        ImGui::End();
    }
}