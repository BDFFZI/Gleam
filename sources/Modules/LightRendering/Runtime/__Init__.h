#pragma once
#include "RenderingConfig.h"
#include "System/LinesRendererSystem.h"
#include "System/PointsRendererSystem.h"
#include "System/RenderingSystem.h"

namespace Light
{
    Light_AddSystems(
        RenderingSystem,
        PointsRendererSystem,
        LinesRendererSystem
    )
}

#include "LightPresentation/Runtime/__Init__.h"

namespace Light
{
    Light_MakeInitEvent()
    {
        CreateGraphicsConfig = RenderingConfig::CreateGraphicsConfig;
    }
}