#include "GBuffer.h"

using namespace Gleam;

GBuffer::GBuffer(size_t size)
{
    glBuffer = std::make_unique<GLBuffer>(
        size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    glBufferAddress = glBuffer->MapMemory();
}