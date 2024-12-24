#pragma once
#include <stack>
#include "Material.h"
#include "RenderTarget.h"
#include "../GL/Pipeline/GLCommandBuffer.h"
#include "LightMath/Runtime/Matrix.hpp"

namespace Light
{
    class CommandBuffer
    {
    public:
        CommandBuffer() : glCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY)
        {
        }
        CommandBuffer(const CommandBuffer&) = delete;

        GLCommandBuffer& GetGLCommandBuffer() { return glCommandBuffer; }
        const MeshAsset* GetCurrentMesh() const { return currentMesh; }
        const ShaderAsset* GetCurrentShader() const { return currentShader; }
        const MaterialAsset* GetCurrentMaterial() const { return currentMaterial; }
        const RenderTargetAsset* GetCurrentRenderTarget() const { return currentRenderTarget; }

        void BeginRecording();
        void EndRecording();

        /**
         * @brief 开始一段绘制操作
         * @param renderTarget 绘制到的目标帧缓冲区
         * @param clearColor 绘制前是否清除颜色（这比单独的清除操作性能更好）
         */
        void BeginRendering(const RenderTargetAsset& renderTarget, bool clearColor = false);
        void EndRendering();

        /**
         * 本质是对BeginRendering和EndRendering的封装调用，目的是为了模仿其他引擎的API习惯
         * @param renderTarget 
         * @note 若要从SetRenderTarget方式切换回Rendering，必须先调用SetRenderTargetToNull
         */
        void SetRenderTarget(const RenderTargetAsset& renderTarget);
        void SetRenderTargetToNull();

        /**
         * 采用左手坐标系，左下为0，右上为渲染目标最大分辨率
         * @param x 
         * @param y 
         * @param width 
         * @param height 
         */
        void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height) const;
        void SetViewportToFullscreen() const;

        void SetViewProjectionMatrices(const float4x4& viewMatrix, const float4x4& projMatrix);
        void SetViewProjectionMatrices(const float4x4& matrixVP);
        void SetViewProjectionMatricesToIdentity();

        void Draw(MeshAsset& mesh, MaterialAsset& material);
        void Draw(MeshAsset& mesh, Material& material, const float4x4& modelMatrix);
        void ClearRenderTarget(const std::optional<float4>& color = 0.0f, const std::optional<float>& depth = 1.0f) const;

    private:
        inline static std::stack<CommandBuffer*> commandBufferPool = {};

        GLCommandBuffer glCommandBuffer;
        const MeshAsset* currentMesh = nullptr;
        const ShaderAsset* currentShader = nullptr;
        const MaterialAsset* currentMaterial = nullptr;
        const RenderTargetAsset* currentRenderTarget = nullptr;
        float4x4 matrixVP = float4x4::Identity();
    };
}
