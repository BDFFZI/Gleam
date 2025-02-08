#pragma once
#include "LightPresentation/Runtime/__Init__.h"

#include "RenderingConfig.h"
#include "System/CameraSystem.h"
#include "System/LinesRendererSystem.h"
#include "System/PointsRendererSystem.h"
#include "System/RenderingSystem.h"

#ifdef Light_Editor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_MakeInitEvent()
    {
        CreateGraphicsConfig = RenderingConfig::CreateGraphicsConfig;
    }
    
    Light_AddSystems(
        RenderingSystem,
        PointsRendererSystem,
        LinesRendererSystem,
        CameraSystem
    )

#ifdef Light_Editor
    Light_AddEditorSystems(
        RenderingSystem,
        PointsRendererSystem,
        LinesRendererSystem,
        CameraSystem
    );
#endif
}