#pragma once
#include "LightGraphics/Runtime/Graphics.h"

namespace Light
{
    class UI
    {
    public:
        static UI Initialize(Graphics&);
        static void UnInitialize();
        static void BeginFrame();
        static void EndFrame(GLCommandBuffer& commandBuffer);

    private:
        inline static std::unique_ptr<GLDescriptorSetLayout> descriptorSetLayout = {};
        inline static std::unique_ptr<GLDescriptorPool> descriptorPool = {};

        UI() = default;
    };
}
