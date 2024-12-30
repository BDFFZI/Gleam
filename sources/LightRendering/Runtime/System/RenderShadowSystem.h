#pragma once
#include "RenderOpaqueSystem.h"

namespace Light
{
    class RenderShadowSystem : public System
    {
    public:
        RenderShadowSystem(): System(RenderOpaqueSystem, OrderRelation::Before)
        {
        }
    };
    Light_MakeSystem(RenderShadowSystem)
}