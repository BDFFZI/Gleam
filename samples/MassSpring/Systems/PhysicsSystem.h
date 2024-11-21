#pragma once

#include "PresentationSystem.h"
#include "LightECS/Runtime/System.hpp"

namespace Light
{
    class Physics
    {
    public:
        using Function = void(*)();

        static float GetFixedDeltaTime() { return fixedDeltaTime; }
        static float2 GetGravity() { return gravity; }

        static void SetFixedDeltaTime(const float fixedDeltaTime) { Physics::fixedDeltaTime = fixedDeltaTime; }
        static void SetGravity(const float gravity) { Physics::gravity = gravity; }

        static void AddFixedUpdate(const Function function) { fixedUpdates.push_back(function); }
        static void RemoveFixedUpdate(const Function function) { fixedUpdates.remove(function); }

    private:
        friend struct BeginPhysicsSystem;
        friend struct EndPhysicsSystem;
        inline static float lastTime = 0;
        inline static float fixedDeltaTime = 0.01f;
        inline static float2 gravity = {0.0f, -9.81f};
        inline static std::list<Function> fixedUpdates = {};
    };

    struct BeginPhysicsSystem : System<MinSystemOrder, BeginPresentationSystem>
    {
    };

    struct EndPhysicsSystem : System<BeginPhysicsSystem, BeginPresentationSystem>
    {
        static void Update();

    private:
        static void CollectForce();
        static void ResolveForce();
        static void UpdateRenderingData();
    };

#define PhysicsSystemGroup Light::BeginPhysicsSystem,Light::EndPhysicsSystem
}
