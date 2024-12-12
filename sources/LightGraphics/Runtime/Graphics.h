#pragma once
#include "CommandBuffer.h"
#include "LightGL/Runtime/GL.h"
#include "LightUtility/Runtime/ObjectPool.hpp"

namespace Light
{
    struct GraphicsConfig
    {
        
    }; 
    
    class Graphics
    {
    public:
        static void InitializeGLDemand(std::vector<const char*>& extensions);

        static Graphics Initialize(
            GL&, VkFormat presentColorFormat, VkFormat presentDepthStencilFormat, VkSampleCountFlagBits presentSampleCount);
        static void UnInitialize();

        static CommandBuffer& ApplyCommandBuffer(const std::string& name = "");
        static void ReleaseCommandBuffer(CommandBuffer& commandBuffer);

    private:
        inline static std::unique_ptr<ObjectPool<CommandBuffer>> paintCommandBufferPool = {};

        Graphics() = default;
    };
}
