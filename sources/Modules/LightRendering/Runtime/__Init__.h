#pragma once
#include "RenderingConfig.h"
#include "System/RenderPostProcessing.h"
#include "System/RenderShadowSystem.h"
#include "LightPresentation/Runtime/__Init__.h"

namespace Light
{
    Light_AddSystems(RenderingSystem)
    Light_AddSystems(RenderShadowSystem)
    Light_AddSystems(RenderOpaqueSystem)
    Light_AddSystems(RenderSkyboxSystem)
    Light_AddSystems(RenderTransparentSystem)
    Light_AddSystems(RenderPostProcessing)
    
    Light_MakeInitEvent()
    {
        CreateGraphicsConfig = RenderingConfig::CreateGraphicsConfig;
    }
}