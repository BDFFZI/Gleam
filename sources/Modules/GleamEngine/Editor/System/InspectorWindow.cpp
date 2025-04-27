#include "InspectorWindow.h"

#include "GleamEngine/Editor/EditorUI/EditorUI.h"
#include "GleamUI/Runtime/UI.h"

namespace Gleam
{
    void InspectorWindow::MakeCustomUI(std::type_index typeIndex, const std::function<void(void*)>& drawInspectorUI)
    {
        inspectorGUIs.insert({typeIndex, drawInspectorUI});
    }
    const InspectorWindow::CustomUI& InspectorWindow::GetCustomUI()
    {
        return inspectorGUIs;
    }
    bool& InspectorWindow::GetIsDebugGUI()
    {
        return isDebugGUI;
    }

    const InspectorTarget& InspectorWindow::GetMajorTarget() const
    {
        return inspectorTargets.front();
    }
    void InspectorWindow::SetMajorTarget(const InspectorTarget& target)
    {
        inspectorTargets.front() = target;
    }
    void InspectorWindow::AddMinorTarget(const InspectorTarget& inspectorTarget)
    {
        inspectorTargets.emplace_back(inspectorTarget);
    }


    void InspectorWindow::Update()
    {
        for (size_t i = 0; i < inspectorTargets.size(); ++i)
        {
            if (i == 0)
                ImGui::Begin("InspectorWindow", nullptr, ImGuiWindowFlags_MenuBar);
            else
            {
                //非默认检视窗口，支持多窗口和关闭功能
                bool isOpen = true;
                ImGui::Begin(std::format("InspectorWindow##{}", i).c_str(), &isOpen);
                if (isOpen == false)
                {
                    inspectorTargets.erase(inspectorTargets.begin() + static_cast<ptrdiff_t>(i));
                    ImGui::End();
                    continue;
                }
            }

            //绘制菜单项
            if (ImGui::BeginMenuBar())
            {
                if (ImGui::MenuItem("Clone"))
                    inspectorTargets.emplace_back(inspectorTargets[i]); //增加元素可能导致扩容，而使指针失效，因此不能一开始就缓存指针或引用
                if (ImGui::MenuItem("Clear"))
                    SetMajorTarget(nullptr);
                if (ImGui::BeginMenu("Debug"))
                {
                    ImGui::Checkbox("UseDebugGUI", &isDebugGUI);
                    ImGui::EndMenu();
                }

                ImGui::EndMenuBar();
            }

            //绘制目标
            InspectorTarget& target = inspectorTargets[i];
            if (!target.objectPtr.expired())
            {
                if (inspectorGUIs.contains(target.objectTypeIndex))
                    inspectorGUIs[target.objectTypeIndex](target.objectPtr.lock().get());
                else
                    EditorUI::DrawSerializedContent(target.objectPtr.lock().get(), target.objectTypeIndex);
            }
            else
                ImGui::Text("Target has expired");

            ImGui::End();
        }
    }
}