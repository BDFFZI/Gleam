#pragma once
#include "LightPresentation/Runtime/__Init__.h"

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

    Light_MakeInitEvent()
    {
        CreateGraphicsConfig = RenderingConfig::CreateGraphicsConfig;
    }
}