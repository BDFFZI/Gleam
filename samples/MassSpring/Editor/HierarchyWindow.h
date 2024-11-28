#pragma once
#include "../Systems/UISystem.h"
#include "LightECS/Runtime/_Template.hpp"

namespace Light
{
    struct HierarchyWindow : SystemT<UISystemGroup>
    {
        static void Update();
    };
}
