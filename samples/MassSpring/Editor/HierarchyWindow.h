#pragma once
#include "../Systems/UISystem.h"
#include "LightECS/Runtime/System.hpp"

struct HierarchyWindow : System<UISystemGroup>
{
    static void Update();
};
