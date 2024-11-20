#pragma once
#include "PhysicsSystem.h"
#include "LightECS/Runtime/Entity.hpp"
#include "LightECS/Runtime/System.hpp"

namespace Light
{
    struct LogicSystem : System<PhysicsSystemGroup>
    {
        static void Start();
        static void Stop();
        static void Update();
        static void FixedUpdate();

    private:
        inline static Entity captivePoint = Entity::Null;
    };
}
