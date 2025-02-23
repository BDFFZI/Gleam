#include "HandlesSystem.h"

#include <imgui.h>
#include <ImGuizmo.h>

namespace Gleam
{
    HandlesSystem& HandlesSystem::GetInstance()
    {
        static HandlesSystem instance = Engine::CreateSystem<HandlesSystem>();
        return instance;
    }

    void HandlesSystem::Start()
    {
        ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
        ImGuizmo::AllowAxisFlip(false); //禁用手柄轴自动反转
        ImGuizmo::SetGizmoSizeClipSpace(0.2f); //设置手柄在剪辑空间的大小
    }
    void HandlesSystem::Update()
    {
        ImGuizmo::BeginFrame();
    }
}