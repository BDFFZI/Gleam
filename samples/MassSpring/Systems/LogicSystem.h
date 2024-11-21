#pragma once
#include "UISystem.h"
#include "LightECS/Runtime/Entity.hpp"
#include "LightECS/Runtime/System.hpp"

namespace Light
{
    struct LogicSystem : System<UISystemGroup>
    {
        inline static Entity captivePoint = Entity::Null;
        inline static float2 mousePositionWS = 0;

        static void Start();
        static void Stop();
        static void Update();
        static void FixedUpdate();
    };
}
