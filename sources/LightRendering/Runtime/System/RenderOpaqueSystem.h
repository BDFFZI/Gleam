#pragma once
#include "RenderSkyboxSystem.h"

namespace Light
{
    class RenderOpaqueSystem : public System
    {
    public:
        RenderOpaqueSystem(): System(RenderSkyboxSystem, OrderRelation::Before)
        {
        }
    };
    Light_MakeSystem(RenderOpaqueSystem)
}