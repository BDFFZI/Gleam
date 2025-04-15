#pragma once

#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

#ifdef GleamRenderingRuntime
#include "GleamRendering/Runtime/System/RenderingSystem.h"
#define UISystemBase public RelativeSystem<RenderingSystem,SystemRelation::After>,public ISystemGroup
#else
#define UISystemBase public System<PresentationSystem>,public ISystemGroup
#endif

namespace Gleam
{
    class UISystem : UISystemBase
    {
        void Update() override;
    };
#ifdef GleamEngineEditor
    Gleam_MakeEditorSystem(UISystem)
#else
    Gleam_MakeRuntimeSystem(UISystem)
#endif
}

#undef UISystemGroup