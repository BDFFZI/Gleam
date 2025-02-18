#include "GBuffer.h"

using namespace Gleam;

GBuffer::GBuffer(size_t size, VkBufferUsageFlags usage)
{
    glBuffer = std::make_unique<GLBuffer>(
        size, usage,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    glBufferAddress = glBuffer->MapMemory();
}

const GLBuffer& GBuffer::GetGLBuffer() const
{
    return *glBuffer;
}
size_t GBuffer::GetSize() const
{
    return glBuffer->size;
}

void GBuffer::SetData(const void* data) const
{
    memcpy(glBufferAddress, data, glBuffer->size);
}