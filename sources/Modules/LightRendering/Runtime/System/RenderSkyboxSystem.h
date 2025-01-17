#pragma once
#include "RenderingSystem.h"

namespace Light
{
    class RenderSkyboxSystem : public System
    {
    public:
        RenderSkyboxSystem(): System(RenderingSystem)
        {
        }
    };
    Light_MakeSystem(RenderSkyboxSystem)
}