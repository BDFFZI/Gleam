#pragma once
#include <memory>

#include "GleamGL/Runtime/Resource/GLBuffer.h"

namespace Gleam
{
    /**
     * @note 不要在渲染时修改缓冲区内容！这是一项vk规范要求。虽然实际上渲染时也可以更新，但由于推送描述符只记录绑定的缓冲区引用而不是内容，
     * 因此内容更新会导致实际渲染时，缓冲区的值与录制时不一致。
     */
    class GBuffer
    {
    public:
        explicit GBuffer(size_t size, VkBufferUsageFlags usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

        void SetData(const void* data) const { memcpy(glBufferAddress, data, glBuffer->size); }

        const GLBuffer& GetGLBuffer() const { return *glBuffer; }

    private:
        std::unique_ptr<GLBuffer> glBuffer;
        void* glBufferAddress;
    };
}