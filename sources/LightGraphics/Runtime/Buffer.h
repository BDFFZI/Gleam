#pragma once
#include <memory>

#include "LightGL/Runtime/Resource/GLBuffer.h"

namespace Light
{
    class Buffer
    {
    public:
        explicit Buffer(size_t size);

        const GLBuffer& GetGLBuffer() const;

        void SetData(const void* data) const;

    private:
        std::unique_ptr<GLBuffer> glBuffer;
        void* glBufferAddress;
    };
}
