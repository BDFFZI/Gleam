#pragma once
#include "PresentationSystem.h"
#include "LightECS/Runtime/System.hpp"

namespace Light
{
    struct BeginUISystem : System<PresentationSystemGroup>
    {
        static void Update();
    };
    struct EndUISystem : System<BeginUISystem, EndPresentationSystem>
    {
        static void Update();
    };

#define UISystemGroup Light::BeginUISystem,Light::EndUISystem
}
