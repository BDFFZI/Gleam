#pragma once
#include "CommandBuffer.h"
#include "GraphicsPreset.h"
#include "LightGL/Runtime/GL.h"
#include "LightUtility/Runtime/ObjectPool.hpp"

namespace Light
{
    class Graphics
    {
    public:
        static void InitializeGLDemand(std::vector<const char*>& extensions);

        static Graphics Initialize(GL&);
        static void UnInitialize();

        static CommandBuffer& ApplyCommandBuffer(const std::string& name = "");
        static void ReleaseCommandBuffer(CommandBuffer& commandBuffer);

        static RenderTargetAsset& GetDefaultRenderTarget();
        static void SetDefaultRenderTarget(RenderTargetAsset& renderTarget);

    private:
        inline static std::unique_ptr<ObjectPool<CommandBuffer>> paintCommandBufferPool = {};
        inline static RenderTargetAsset* defaultRenderTarget = nullptr;

        Graphics() = default;
    };
}
