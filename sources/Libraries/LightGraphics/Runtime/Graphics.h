#pragma once
#include "CommandBuffer.h"
#include "LightGL/Runtime/GL.h"
#include "LightUtility/Runtime/ObjectPool.hpp"
#include "Resource/GTexture.h"

namespace Light
{
    class Graphics
    {
    public:
        static void InitializeGLDemand(std::vector<const char*>& extensions);

        static Graphics Initialize(GL&, const GraphicsPreset& defaultPreset);
        static void UnInitialize();

        static CommandBuffer& ApplyCommandBuffer(const std::string& name = "");
        static void ReleaseCommandBuffer(CommandBuffer& commandBuffer);

        static GLImageSampler& GetDefaultGLImageSampler() { return *defaultGLImageSampler; }
        static RenderTargetAsset& GetDefaultRenderTarget() { return *defaultRenderTarget; }

        static void SetDefaultRenderTarget(RenderTargetAsset& renderTarget) { defaultRenderTarget = &renderTarget; }

    private:
        inline static std::unique_ptr<ObjectPool<CommandBuffer>> paintCommandBufferPool = {};
        //预创建的默认资源
        inline static GraphicsPreset defaultGraphicsPreset = {};
        inline static std::unique_ptr<GLDescriptorSetLayout> defaultGLDescriptorSetLayout = {};
        inline static std::unique_ptr<GLPipelineLayout> defaultGLPipelineLayout = {};
        inline static std::unique_ptr<GLImageSampler> defaultGLImageSampler = {};
        inline static std::unique_ptr<GTexture2D> defaultTexture2D = {};
        inline static RenderTargetAsset* defaultRenderTarget = nullptr;

        Graphics() = default;
    };
}