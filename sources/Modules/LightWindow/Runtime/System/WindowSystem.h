#pragma once

#include "LightECS/Runtime/System.h"
#include "LightEngine/Runtime/System/UpdateSystem.h"
#include "LightMath/Runtime/LinearAlgebra/Vector.h"

namespace Light
{
    class WindowSystem : public SystemGroup
    {
    public:
        WindowSystem(): SystemGroup(PreUpdateSystem, LeftOrder, MiddleOrder)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystemInstance(WindowSystem)
}