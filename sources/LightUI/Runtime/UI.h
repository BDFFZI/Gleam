#pragma once
#include <imgui_impl_glfw.h>

#define IM_VEC2_CLASS_EXTRA ImVec2(Light::float2 value){x=value.x;y=value.y;} \
operator Light::float2()const{return{x,y};}
#define IM_VEC4_CLASS_EXTRA ImVec4(BDXKEngine::Color value){x=value.r;y=value.g;z=value.b;w=value.a;} \
operator BDXKEngine::Color()const{return{x,y,z,w};}

#include <imgui.h>

#include "LightGraphics/Runtime/Graphics.h"
#include "LightWindow/Runtime/Input.h"

float2 a = static_cast<float2>(ImVec2());

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
