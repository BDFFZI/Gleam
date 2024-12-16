#pragma once
// ReSharper disable CppUnusedIncludeDirective
#define IM_VEC2_CLASS_EXTRA ImVec2(Light::float2 value){x=value.x;y=value.y;} \
operator Light::float2()const{return Light::float2{x,y};}
#define IM_VEC4_CLASS_EXTRA ImVec4(Light::float4 value){x=value.x;y=value.y;z=value.z;w=value.w;} \
operator Light::float4()const{return Light::float4{x,y,z,w};}
#include "LightMath/Runtime/Vector.hpp"
#include <imgui.h>
// ReSharper restore CppUnusedIncludeDirective

#include <imgui_impl_glfw.h>
#include "LightGraphics/Runtime/Graphics.h"
#include "LightWindow/Runtime/Input.h"

namespace Light
{
    class UI
    {
    public:
        static UI Initialize(Window&, Graphics&);
        static void UnInitialize();
        static void BeginFrame();
        static void EndFrame(GLCommandBuffer& commandBuffer);

        static ImTextureID CreateTexture(TextureAsset& texture);
        static void DeleteTexture(ImTextureID texture);

        static float2 GetWindowContentRegionSize();

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
