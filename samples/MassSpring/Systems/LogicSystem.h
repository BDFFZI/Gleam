#pragma once
#include "UISystem.h"
#include "LightECS/Runtime/_Concept.hpp"
#include "LightECS/Runtime/_Template.hpp"

namespace Light
{
    struct LogicSystem : SystemT<UISystemGroup>
    {
        inline static Entity captivePoint = Entity::Null;
        inline static float2 mousePositionWS = 0;

        static void Start();
        static void Stop();
        static void Update();
        static void FixedUpdate();
    };
}
