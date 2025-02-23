#pragma once
// ReSharper disable CppUnusedIncludeDirective
#define IM_VEC2_CLASS_EXTRA ImVec2(Gleam::float2 value){x=value.x;y=value.y;} \
operator Gleam::float2()const{return Gleam::float2{x,y};}
#define IM_VEC4_CLASS_EXTRA ImVec4(Gleam::float4 value){x=value.x;y=value.y;z=value.z;w=value.w;} \
operator Gleam::float4()const{return Gleam::float4{x,y,z,w};}
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include <imgui.h>
#include <magic_enum.hpp>
// ReSharper restore CppUnusedIncludeDirective

#include "GleamGL/Runtime/Resource/GLDescriptorPool.h"
#include "GleamGraphics/Runtime/Resource/GTexture/GTexture.h"
#include "GleamMath/Runtime/LinearAlgebra/Matrix.h"

namespace Gleam
{
    class UI
    {
    public:
        static void Init();
        static void UnInit();
        static void BeginFrame();
        static void EndFrame();

        static ImTextureID CreateTexture(GTexture& texture);
        static void DeleteTexture(ImTextureID& texture);
        static float2 GetWindowContentRegionSize();

        static bool DragFloat4x4(const char* label, float4x4* v, float v_speed = 1);
        /**
         * 按路径层级绘制菜单项，会自动执行Begin/EndMenu来绘制中间层
         * @param path 
         * @param func 
         * @param layer 
         */
        static void MenuItem(const std::vector<std::string>& path, const std::function<void()>& func, size_t layer = 0);

    private:
        static inline std::unique_ptr<GLDescriptorSetLayout> descriptorSetLayout = {};
        static inline std::unique_ptr<GLDescriptorPool> descriptorPool = {};
    };
}