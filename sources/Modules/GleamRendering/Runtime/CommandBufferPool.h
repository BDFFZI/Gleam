#pragma once
#include <memory>

#include "Asset/CommandBuffer.h"
#include "GleamUtility/Runtime/Pool/ObjectPool.h"

namespace Gleam
{
    class CommandBufferPool
    {
    public:
        static void Init();
        static void UnInit();

        static CommandBuffer& Apply(const std::string& name = "");
        static void Release(CommandBuffer& commandBuffer);

    private:
        inline static std::unique_ptr<ObjectPool<CommandBuffer>> commandBuffers = std::make_unique<ObjectPool<CommandBuffer>>();
    };
}