#pragma once
#include "../Public/SimulationSystem.h"
#include "LightECS/Runtime/System.h"
#include "LightMath/Runtime/Vector.h"

namespace Light
{
    class PhysicsSystem : public SystemGroup
    {
    public:
        PhysicsSystem(): SystemGroup(&SimulationSystem, LeftOrder, MiddleOrder)
        {
        }

        float2 GetGravity() const { return gravity; }
        float GetFixedDeltaTime() const { return fixedDeltaTime; }

    private:
        float lastTime = 0;
        float fixedDeltaTime = 0.01f;
        float2 gravity = {0.0f, -9.81f};

        void Update() override;
    };
    inline PhysicsSystem PhysicsSystem = {};
}
