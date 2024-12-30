#pragma once
#include "RenderSkyboxSystem.h"

namespace Light
{
    class RenderTransparentSystem : public System
    {
    public:
        RenderTransparentSystem(): System(RenderSkyboxSystem, OrderRelation::After)
        {
        }
    };
    Light_MakeSystem(RenderTransparentSystem)
}