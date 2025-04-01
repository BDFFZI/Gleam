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

    /**
     * UI子系统模板，便于快速编写自定义UI
     */
    class UISubSystem : public System
    {
    public:
        UISubSystem(): System(GlobalUISystem)
        {
        }
    };
}

#undef Order