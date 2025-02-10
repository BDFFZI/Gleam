#pragma once
#include <memory>

#include "GleamGL/Runtime/Resource/GLBuffer.h"

namespace Gleam
{
    class GBuffer
    {
    public:
        explicit GBuffer(size_t size);

        void SetData(const void* data) const { memcpy(glBufferAddress, data, glBuffer->size); }

        const GLBuffer& GetGLBuffer() const { return *glBuffer; }

    private:
        std::unique_ptr<GLBuffer> glBuffer;
        void* glBufferAddress;
    };
}