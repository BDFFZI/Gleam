#pragma once
#include "LightECS/Runtime/System.h"
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
