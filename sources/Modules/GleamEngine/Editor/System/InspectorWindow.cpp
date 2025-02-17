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
    const CustomGUI& InspectorWindow::GetCustomGUI()
    {
        return inspectorGUIs;
    }
    void InspectorWindow::AddCustomGUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorGUI)
    {
        inspectorGUIs.insert({typeIndex, drawInspectorGUI});
    }

    Entity InspectorWindow::GetTargetEntity()
    {
        if (GetTargetType() == typeid(Entity))
            return *std::get<0>(target)._Cast<Entity>();

        return Entity::Null;
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
            else
            {
                EditorUISerializer serializer = {"InspectionTarget"};
                Type& type = Type::GetType(targetType);
                if (type.Serialize()) //序列化每个元素
                    type.Serialize()(serializer, targetPtr);
                else //未知类型，当成字段整体传输给序列化器判断
                    serializer.Transfer(targetPtr, targetType);
            }
        }

        ImGui::End();
    }
}