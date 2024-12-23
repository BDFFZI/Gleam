#pragma once
#include "PhysicsSystem.h"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class ForceSystem : public System
    {
    public:
        ForceSystem(): System(&PhysicsSystem, LeftOrder, MiddleOrder)
        {
        }

        void Update() override;
    };
    inline ForceSystem ForceSystem = {};
}
