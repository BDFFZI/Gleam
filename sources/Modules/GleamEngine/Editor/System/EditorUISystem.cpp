#include "EditorUISystem.h"

#include "GleamUI/Runtime/UI.h"
#include <ImGuizmo.h>

#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamUtility/Runtime/String.h"

namespace Gleam
{
    void EditorUISystem::AddEditorMenu(const std::string& name, const std::function<void()>& action)
    {
        editorMenus.emplace_back(name, action);
    }

    std::vector<std::string> path = {};
    void EditorUISystem::Start()
    {
        //初始化ImGuizmo
        ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
        ImGuizmo::AllowAxisFlip(false); //禁用手柄轴自动反转
        ImGuizmo::SetGizmoSizeClipSpace(0.2f); //设置手柄在剪辑空间的大小
        
        SystemGroup::Start();
    }
    void EditorUISystem::Update()
    {
        ImGui::ShowIDStackToolWindow();
        
        //启动ImGuizmo
        ImGuizmo::BeginFrame();
        //增加船坞功能
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        //绘制菜单项
        if (ImGui::BeginMainMenuBar())
        {
            //系统菜单项
            bool isPlaying = Editor::IsPlaying();
            if (ImGui::Checkbox("IsPlaying", &isPlaying))
                Editor::IsPlaying() = isPlaying;
            bool isPausing = !GlobalTimeSystem.GetAutoStepTime();
            if (ImGui::Checkbox("IsPausing", &isPausing))
                GlobalTimeSystem.SetAutoStepTime(!isPausing);
            if (ImGui::Button("NextFrame"))
            {
                GlobalTimeSystem.SetAutoStepTime(false);
                GlobalTimeSystem.SetStepTime(GlobalTimeSystem.GetFixedDeltaTime());
            }

            //自定义菜单项
            for (auto& [name,func] : editorMenus)
            {
                String::Split(name, "/", path);
                UI::MenuItem(path, func);
            }

            ImGui::EndMainMenuBar();
        }

        //绘制其他界面
        SystemGroup::Update();
    }
}