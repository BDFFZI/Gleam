#include "InspectorWindow.h"

#include "EditorUI/EditorUISerializer.h"
#include "LightECS/Runtime/World.h"
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

    void* InspectorWindow::GetTarget()
    {
        void* targetPtr;
        switch (target.index())
        {
        case 0:
            targetPtr = &std::get<std::any>(target);
            break;
        case 1:
            targetPtr = std::get<void*>(target);
            break;
        case 2:
            targetPtr = std::get<std::weak_ptr<void>>(target).lock().get();
            break;
        default:
            throw std::runtime_error("不支持的目标种类！");
        }
        return targetPtr;
    }
    std::type_index InspectorWindow::GetTargetType() const
    {
        return targetType;
    }
    void InspectorWindow::SetTarget(const std::variant<std::any, void*, std::weak_ptr<void>>& target, const std::type_index targetType)
    {
        this->target = target;
        this->targetType = targetType;
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
                std::format("InspectorWindow##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
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
        void* targetPtr = GetTarget();
        if (targetPtr != nullptr)
        {
            if (inspectorGUIs.contains(targetType))
                inspectorGUIs[targetType](targetPtr);
            else if (Type* type = Type::GetType(targetType))
            {
                EditorUISerializer serializer = {};
                type->serialize(serializer, targetPtr);
            }
            else
                ImGui::Text("The inspected target is unknown type.");
        }

        ImGui::End();
    }
}