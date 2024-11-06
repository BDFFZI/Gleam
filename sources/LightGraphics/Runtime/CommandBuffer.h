#pragma once
#include <stack>

#include "Material.h"
#include "RenderTexture.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"

namespace Light
{
    class CommandBuffer
    {
    public:
        CommandBuffer() = default;
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
        void SetViewProjectionMatrices(const float4x4& view, const float4x4& proj);
        void Draw(const MeshBase& mesh, const float4x4& matrix, const Material& material);
        void ClearRenderTexture(float4 color = 0, float depth = 0) const;

    private:
        inline static std::stack<CommandBuffer*> commandBufferPool = {};

        GLCommandBuffer glCommandBuffer = GLCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY);
        const MeshBase* lastMesh = nullptr;
        const Material* lastMaterial = nullptr;
        const Shader* lastShader = nullptr;
        const RenderTextureBase* lastRenderTexture = nullptr;
        PushConstant pushConstant = {};
    };
}
