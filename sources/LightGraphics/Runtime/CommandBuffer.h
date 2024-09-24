#pragma once
#include "Material.h"
#include "Mesh.h"
#include "RenderTarget.h"
#include "LightMath/Runtime/Graphics.h"
#include "LightMath/Runtime/Matrix.h"

namespace LightRuntime
{
    class CommandBuffer
    {
    public:
        void SetRenderPass(const RenderPass& renderPass);
        void SetRenderTarget(const RenderTarget& renderTarget);
        void SetViewport(const rect& viewport);
        void SetViewProjectionMatrices(const float4x4& worldToView, const float4x4& viewToClip);
        void Draw(const MeshBase& mesh, const float4x4& localToWorld, const MaterialBase& material);
    };
}
