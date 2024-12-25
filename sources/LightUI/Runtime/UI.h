#pragma once
// ReSharper disable CppUnusedIncludeDirective
#define IM_VEC2_CLASS_EXTRA ImVec2(Light::float2 value){x=value.x;y=value.y;} \
operator Light::float2()const{return Light::float2{x,y};}
#define IM_VEC4_CLASS_EXTRA ImVec4(Light::float4 value){x=value.x;y=value.y;z=value.z;w=value.w;} \
operator Light::float4()const{return Light::float4{x,y,z,w};}
#include "LightMath/Runtime/Vector.hpp"
#include <imgui.h>
// ReSharper restore CppUnusedIncludeDirective

#include "LightPresentation/Runtime/Graphics/GraphicsAssets.h"

namespace Light
{
    class UI
    {
    public:
        static ImTextureID CreateTexture(TextureAsset& texture);
        static void DeleteTexture(ImTextureID texture);

        static float2 GetWindowContentRegionSize();
    };
}