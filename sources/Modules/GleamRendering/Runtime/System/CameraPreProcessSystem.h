#pragma once
#include "RenderingSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Editor.h"
#endif

namespace Gleam
{
    class CameraPreProcessSystem : public RelativeSystem<RenderingSystem, SystemRelation::Before>
    {
        void Update() override;
    };
    
#ifdef GleamEngineEditor
    Gleam_MakeEditorSystem(CameraPreProcessSystem)
#else
    Gleam_MakeRuntimeSystem(CameraPreProcessSystem)
#endif
}