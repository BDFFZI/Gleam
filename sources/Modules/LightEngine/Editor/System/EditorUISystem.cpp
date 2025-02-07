#include "EditorUISystem.h"

#include "EditorSystem.h"
#include "LightEngine/Runtime/Engine.h"
#include "LightUI/Runtime/UI.h"
#include "LightUtility/Runtime/String.h"

namespace Light
{
    void EditorUISystem::RegisterEditorMenu(std::string name, std::function<void()> action)
    {
        editorMenus.emplace_back(name, action);
    }

    std::vector<std::string> path = {};
    void EditorUISystem::Update()
    {
        //增加船坞功能
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
        //绘制菜单项
        ImGui::PushStyleColor(ImGuiCol_MenuBarBg, EditorSystem->IsPlaying() ? float4::DarkGreen() : float4::Blue());
        if (ImGui::BeginMainMenuBar())
        {
            for (auto& [name,func] : editorMenus)
            {
                String::Split(name, "/", path);
                UI::MenuItem(path, func);
            }

            ImGui::EndMainMenuBar();
        }
        ImGui::PopStyleColor();
        
        //绘制其他界面
        SystemGroup::Update();
    }
}