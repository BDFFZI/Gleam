#pragma once
#include <stack>
#include "LightGL/Runtime/GLCommandBuffer.h"
#include "LightMath/Runtime/Matrix.h"
#include "LightMath/Runtime/Geometry/Rect.h"
#include "Resource/GMaterial.h"
#include "Resource/GMesh.h"
#include "Resource/GRenderTarget/GRenderTexture.h"

namespace Light
{
    class GCommandBuffer
    {
    public:
        GCommandBuffer(std::string_view name = "");
        virtual ~GCommandBuffer() = default;
        GCommandBuffer(const GCommandBuffer&) = delete;
        GCommandBuffer& operator=(const GCommandBuffer&) = delete;
        GCommandBuffer(GCommandBuffer&&) = delete;
        GCommandBuffer& operator=(GCommandBuffer&&) = delete;
        operator GLCommandBuffer&();

        GLCommandBuffer& GetGLCommandBuffer() { return glCommandBuffer; }
        const GMesh* GetCurrentMesh() const { return currentMesh; }
        const GShader* GetCurrentShader() const { return currentShader; }
        const GMaterial* GetCurrentMaterial() const { return currentMaterial; }
        const GRenderTarget* GetCurrentRenderTarget() const { return currentRenderTarget; }

        virtual void BeginRecording();
        virtual void EndRecording();

        /**
         * @brief 开始一段绘制操作
         * @param renderTarget 绘制到的目标帧缓冲区
         * @param clearColor 绘制前是否清除颜色（这比单独的清除操作性能更好）
         */
        void BeginRendering(const GRenderTarget& renderTarget, bool clearColor = false);
        void EndRendering();

        /**
         * 本质是对BeginRendering和EndRendering的封装调用，目的是为了模仿其他引擎的API习惯
         * @param renderTarget 
         * @note 若要从SetRenderTarget方式切换回Rendering，必须先调用SetRenderTargetToNull
         */
        void SetRenderTarget(const GRenderTarget& renderTarget);
        void SetRenderTargetToNull();

        /**
         * 采用右手坐标系，左上为0，右下为渲染目标最大分辨率
         * @param x 
         * @param y 
         * @param width 
         * @param height 
         */
        void SetViewport(int32_t x, int32_t y, uint32_t width, uint32_t height);
        void SetViewportToFullscreen();

        void DrawMesh(GMesh& mesh, GMaterial& material, const std::string_view& shaderPass = "");
        void ClearRenderTarget(const std::optional<float4>& color = 0.0f, const std::optional<float>& depth = 1.0f) const;

    private:
        inline static std::stack<GCommandBuffer*> commandBufferPool = {};

        std::string name;
        GLCommandBuffer glCommandBuffer;
        VkRect2D currentViewport = {};
        const GRenderTarget* currentRenderTarget = nullptr;
        const GMesh* currentMesh = nullptr;
        const GShader* currentShader = nullptr;
        const GMaterial* currentMaterial = nullptr;
    };
}