#pragma once
#include "../Systems/UISystem.h"
#include "LightECS/Runtime/System.hpp"

struct InspectorWindow : System<UISystemGroup>
{
    inline static void* inspectingTarget = nullptr;
    inline static void (*inspectingMethod)() = nullptr;

    static void Update();
    static void InspectEntity();
};
