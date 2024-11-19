#pragma once
#include <imgui_impl_glfw.h>

#include "LightGraphics/Runtime/Graphics.h"
#include "LightWindow/Runtime/Input.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    class UI
    {
    public:
        static UI Initialize(Window&, Graphics&);
        static void UnInitialize();
        static void BeginFrame();
        static void EndFrame(GLCommandBuffer& commandBuffer);

    private:
        inline static std::unique_ptr<GLDescriptorSetLayout> descriptorSetLayout = {};
        inline static std::unique_ptr<GLDescriptorPool> descriptorPool = {};
        inline static InputHandler inputHandler = {
            "UI", nullptr,
            ImGui_ImplGlfw_WindowFocusCallback,
            ImGui_ImplGlfw_CursorEnterCallback,
            ImGui_ImplGlfw_CursorPosCallback,
            ImGui_ImplGlfw_MouseButtonCallback,
            ImGui_ImplGlfw_ScrollCallback,
            ImGui_ImplGlfw_KeyCallback,
            ImGui_ImplGlfw_CharCallback,
            ImGui_ImplGlfw_MonitorCallback
        };

        UI() = default;
    };
}
