#include "Buffer.h"

using namespace LightRuntime;

Buffer::Buffer(int size)
{
    glBuffer = std::make_unique<GLBuffer>(
        size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );
    glBufferAddress = glBuffer->MapMemory();
}

const GLBuffer& Buffer::GetGLBuffer() const
{
    return *glBuffer;
}

void Buffer::SetData(const void* data) const
{
    memcpy(glBufferAddress, data, glBuffer->size);
}
