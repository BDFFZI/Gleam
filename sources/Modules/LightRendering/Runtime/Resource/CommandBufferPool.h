#pragma once
#include <memory>

#include "CommandBuffer.h"
#include "LightUtility/Runtime/Pool/ObjectPool.h"

namespace Light
{
    class CommandBufferPool
    {
    public:
        static CommandBuffer& Apply(const std::string& name = "");
        static void Release(CommandBuffer& commandBuffer);
        static void Clear();

    private:
        inline static std::unique_ptr<ObjectPool<CommandBuffer>> commandBuffers = std::make_unique<ObjectPool<CommandBuffer>>();
    };
}