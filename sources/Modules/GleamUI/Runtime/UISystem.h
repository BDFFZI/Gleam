#pragma once

#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamECS/Runtime/System.h"

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
    public:
        UISystem(): SystemGroup(Order)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(UISystem)
}

#undef Order