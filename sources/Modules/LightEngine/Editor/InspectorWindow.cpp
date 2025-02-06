#include "InspectorWindow.h"

#include "LightECS/Runtime/World.h"
#include "LightReflection/Runtime/Type.h"
#include "LightUI/Runtime/UI.h"

namespace Light
{
    void InspectorWindow::RegisterInspectorGUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorGUI)
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
            ImGui::Begin("InspectorWindow");
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

        if (inspectorGUIs.contains(targetType))
            inspectorGUIs[targetType](target);
        else
            ImGui::Text("The target being inspected is unknown type.");

        ImGui::End();
    }
}