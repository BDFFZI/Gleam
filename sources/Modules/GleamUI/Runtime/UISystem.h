#pragma once

#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

#ifdef GleamRenderingRuntime
#include "GleamRendering/Runtime/System/RenderingSystem.h"
#define Order GlobalRenderingSystem,OrderRelation::After
#else
#define Order GlobalPresentationSystem
#endif

namespace Gleam
{
    class UISystem : public SystemGroup
    {
        Gleam_MakeType_Friend

        void Update() override;

        UISystem(): SystemGroup(Order)
        {
        }
    };
    Gleam_MakeGlobalSystem(UISystem)
}

#undef Order