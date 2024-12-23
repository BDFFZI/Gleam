#pragma once
#include "LightEngine/Runtime/ECS/System.h"
#include "Physics/PhysicsSystem.h"

class FixedPointSystem : public Light::System
{
public:
    FixedPointSystem(): System(&Light::PhysicsSystem, RightOrder)
    {
    }

    void Update() override;
};
inline FixedPointSystem FixedPointSystem = {};
