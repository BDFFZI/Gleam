#pragma once
#include "Rendering.h"
#include "RenderingConfig.h"
#include "System/CameraSystem.h"
#include "System/LinesRendererSystem.h"
#include "System/PointsRendererSystem.h"
#include "System/RenderingSystem.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        CreateGraphicsConfig = RenderingConfig::CreateGraphicsConfig;
    }

    Gleam_AddStartEvent(InitRenderingLibrary, InitGraphicsLibraryOrder+1)
    {
        Rendering::Init();
    }

    Gleam_AddStopEvent(UnInitRenderingLibrary, UnInitGraphicsLibraryOrder-1)
    {
        Rendering::UnInit();
    }

    Gleam_AddSystems(
        RenderingSystem,
        PointsRendererSystem,
        LinesRendererSystem,
        CameraSystem
    )

#ifdef GleamEngineEditor
    Gleam_AddEditorSystems(
        RenderingSystem,
        PointsRendererSystem,
        LinesRendererSystem,
        CameraSystem
    );
#endif
}