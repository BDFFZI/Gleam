#pragma once
#include "LightECS/Runtime/Scene.h"
#include "LightECS/Runtime/World.h"

namespace Light
{
    inline Scene* Awake = World::CreateScene("Awake");
    inline Scene* Destroy = World::CreateScene("Destroy");

    class Engine
    {
    public:
        static void Start();
        static void Stop();

    private:
        inline static bool isStopping = false;
    };
}

// ReSharper disable once CppUnusedIncludeDirective
#include "LightUtility/Runtime/Program.h"
#define Light_AddSystems(...) Light_MakeInitEvent(){::Light::World::AddSystems({__VA_ARGS__});}