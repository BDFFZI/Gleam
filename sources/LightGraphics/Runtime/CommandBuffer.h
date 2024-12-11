#pragma once
#include <stack>
#include "Material.h"
#include "RenderTexture.h"
#include "LightGL/Runtime/Pipeline/GLCommandBuffer.h"
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
        const RenderTargetBase* GetRenderTarget() const { return lastRenderTarget; }

        void BeginRecording();
        void EndRecording();

        /**
         * @brief 开始一段绘制操作
         * @param renderTarget 绘制到的目标帧缓冲区
         * @param clearColor 绘制前是否清除颜色（这比单独的清除操作性能更好）
         */
        void BeginRendering(const RenderTargetBase& renderTarget, bool clearColor = false);
        void EndRendering();

        /**
         * 本质是对BeginRendering和EndRendering的封装调用，目的是为了模仿其他引擎的API习惯
         * @param renderTarget 
         * @note 若要从SetRenderTarget方式切换回Rendering，必须先调用SetRenderTargetToNull
         */
        void SetRenderTarget(const RenderTargetBase& renderTarget);
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

        void Draw(MeshBase& mesh, MaterialBase& material);
        void ClearRenderTarget(const std::optional<float4>& color = 0.0f, const std::optional<float>& depth = 1.0f) const;

    private:
        inline static std::stack<CommandBuffer*> commandBufferPool = {};

        GLCommandBuffer glCommandBuffer;
        const MeshBase* lastMesh = nullptr;
        const MaterialBase* lastMaterial = nullptr;
        const RenderTargetBase* lastRenderTarget = nullptr;
    };
}
