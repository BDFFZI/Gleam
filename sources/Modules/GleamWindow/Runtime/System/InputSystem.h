#pragma once

#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMath/Runtime/Geometry/2D/Rectangle.h"
#include "GleamWindow/Runtime/Entity/Archetype.h"
#include "GleamWindow/Runtime/Entity/Input.h"

namespace Gleam
{
    /**
     * 每帧将GLFW传入的用户输入解析成Gleam所用的输入格式
     */
    class InputSystem : public SystemT<PreUpdateSystem>
    {
    public:
        Input& GetMainInput() const
        {
            return entities->GetComponent<Input>(mainInput);
        }

    private:
        EntityAllocator* entities = nullptr;
        View<Input> view = {};
        Entity mainInput = {};

        void Start() override;
        void Update() override;
    };
    Gleam_MakeSystem(InputSystem)
}