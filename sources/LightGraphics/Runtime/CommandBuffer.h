#pragma once
#include <stack>

#include "GraphicsConfig.hpp"
#include "RenderTexture.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{
    class CommandBuffer
    {
    public:
        CommandBuffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_SECONDARY) : glCommandBuffer(level)
        {
        }
        CommandBuffer(const CommandBuffer&) = delete;

        GLCommandBuffer& GetGLCommandBuffer();

        void BeginRecording();
        void EndRecording();

        /**
         * @brief 开始一段绘制操作
         * @param renderTarget 绘制到的目标帧缓冲区
         * @param clearColor 绘制前是否清除颜色（这比单独的清除操作性能更好）
         */
        void BeginRendering(const RenderTextureBase& renderTarget, bool clearColor = false);
        void EndRendering() const;

        void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height) const;
        void SetViewProjectionMatrices(const float4x4& viewMatrix, const float4x4& projMatrix);
        void Draw(MeshBase* mesh, const float4x4& modelMatrix, MaterialBase* material);
        void ClearRenderTexture(float4 color = 0, float depth = 0) const;

    private:
        inline static std::stack<CommandBuffer*> commandBufferPool = {};

        GLCommandBuffer glCommandBuffer;
        const MeshBase* lastMesh = nullptr;
        const MaterialBase* lastMaterial = nullptr;
        const RenderTextureBase* lastRenderTexture = nullptr;
        float4x4 matrixVP;
    };
}
