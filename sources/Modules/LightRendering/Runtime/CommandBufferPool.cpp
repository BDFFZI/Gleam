#include "CommandBufferPool.h"

namespace Light
{
    void CommandBufferPool::Init()
    {
        commandBuffers = std::make_unique<ObjectPool<CommandBuffer>>();
    }
    void CommandBufferPool::UnInit()
    {
        commandBuffers.reset();
    }
    CommandBuffer& CommandBufferPool::Apply(const std::string& name)
    {
        return commandBuffers->Get();
    }
    void CommandBufferPool::Release(CommandBuffer& commandBuffer)
    {
        commandBuffers->Release(commandBuffer);
    }
}