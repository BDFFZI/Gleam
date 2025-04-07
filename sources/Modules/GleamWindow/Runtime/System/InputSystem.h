#pragma once

#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMath/Runtime/Geometry/2D/Rectangle.h"
#include "GleamWindow/Runtime/Entity/Archetype.h"

namespace Gleam
{
    struct Input;
    /**
     * 每帧将GLFW传入的用户输入解析成Gleam所用的输入格式
     */
    class InputSystem : public SystemT<PreUpdateSystem>
    {
    public:
        Input& GetDefaultInput() const
        {
            return GetAllocator().GetComponent<Input>(defaultInput);
        }

    private:
        Entity defaultInput = {};
        View<Input> view = {};

        void Start() override
        {
            defaultInput = GetAllocator().AddEntity(InputData);
            view = GetView<Input>();
        }
        void Update() override;
    };
    Gleam_MakeSystem(InputSystem)
}