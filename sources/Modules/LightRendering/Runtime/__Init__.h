#pragma once
#include "Rendering.h"
#include "LightPresentation/Runtime/__Init__.h"

#include "RenderingConfig.h"
#include "System/CameraSystem.h"
#include "System/LinesRendererSystem.h"
#include "System/PointsRendererSystem.h"
#include "System/RenderingSystem.h"

#ifdef LightEngineEditor
#include "LightEngine/Editor/Editor.h"
#endif

namespace Light
{
    Light_MakeInitEvent()
    {
        CreateGraphicsConfig = RenderingConfig::CreateGraphicsConfig;
    }

    Light_AddStartEvent(InitRenderingLibrary, InitGraphicsLibraryOrder+1)
    {
        Rendering::Init();
    }

    Light_AddStopEvent(UnInitRenderingLibrary, UnInitGraphicsLibraryOrder-1)
    {
        Rendering::UnInit();
    }

    Light_AddSystems(
        RenderingSystem,
        PointsRendererSystem,
        LinesRendererSystem,
        CameraSystem
    )

#ifdef LightEngineEditor
    Light_AddEditorSystems(
        RenderingSystem,
        PointsRendererSystem,
        LinesRendererSystem,
        CameraSystem
    );
#endif
}