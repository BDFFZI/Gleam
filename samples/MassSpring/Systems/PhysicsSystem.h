#pragma once
#include "PresentationSystem.h"
#include "SimulationSystem.h"
#include "LightECS/Runtime/System.hpp"

namespace Light
{
    struct PhysicsSystem : System<SimulationSystemGroup>
    {
        constexpr static float DeltaTime = 0.02f;

        static void Update();

    private:
        inline static float lastTime = 0;

        static void UpdateSpring();
        static void UpdateMassPoint();
        static void UpdateRenderingData();
    };
}
