#pragma once
#include "PresentationSystem.h"
#include "LightECS/Runtime/_Template.hpp"

namespace Light
{
    struct BeginUISystem : SystemT<PresentationSystemGroup>
    {
        static void Update();
    };
    struct EndUISystem : SystemT<BeginUISystem::Order, EndPresentationSystem::Order>
    {
        static void Update();
    };

#define UISystemGroup Light::BeginUISystem::Order,Light::EndUISystem::Order
}
