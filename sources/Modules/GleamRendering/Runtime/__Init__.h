#pragma once
#include "Rendering.h"
#include "RenderingConfig.h"
#include "System/CameraPreProcessSystem.h"
#include "System/LinesRendererPreProcessSystem.h"
#include "System/PointsRendererPreProcessSystem.h"
#include "System/RenderingSystem.h"
#include "Entity/Archetype.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    Gleam_MakeInitEvent()
    {
        CreateGraphicsConfig = RenderingConfig::CreateGraphicsConfig;
    }

    Gleam_MakeEngineStartEvent(InitRenderingLibrary, InitGraphicsLibraryOrder+1)
    {
        Rendering::Init();
    }

    Gleam_MakeEngineStopEvent(UnInitRenderingLibrary, UnInitGraphicsLibraryOrder-1)
    {
        Rendering::UnInit();
    }
}