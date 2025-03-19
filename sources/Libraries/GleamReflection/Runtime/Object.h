#pragma once
#include "Type.h"

namespace Gleam
{
    /**
     * 可反射物体的访问包装器
     */
    class Object
    {
    public:
        Object(void* target, const Type& targetType);

        //未来可能将其设计为真正的独立Object托管器，而不是仅仅的访问
        Object(Object& other) = delete;
        Object& operator=(const Object& other) = delete;

    private:
        void* target;
        const Type* targetType;
    };
}