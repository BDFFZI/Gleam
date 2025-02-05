#include "CommandBufferPool.h"

namespace Light
{
    CommandBuffer& CommandBufferPool::Apply(const std::string& name)
    {
        return commandBuffers->Get();
    }
    void CommandBufferPool::Release(CommandBuffer& commandBuffer)
    {
        commandBuffers->Release(commandBuffer);
    }
    void CommandBufferPool::Clear()
    {
        commandBuffers.reset();
    }
}