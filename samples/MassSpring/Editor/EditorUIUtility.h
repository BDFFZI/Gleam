#pragma once
#include <typeindex>

#include "LightEngine/Runtime/ECS/System.h"
#include "LightEngine/Runtime/ECS/_Concept.hpp"
#include "LightEngine/Runtime/Reflection/Serialization/DataTransferrer.hpp"

namespace Light
{
    class EditorUIUtility
    {
    public:
        /**
         * 
         * @param entity
         * @note 注意：必须确保entity变量是长期有效的，不能使用临时值！（因为内部使用指针进行引用）
         */
        static void DrawEntityButton(Entity entity);
        static void DrawSystemGroup(SystemGroup& systemGroup);
    };
}
