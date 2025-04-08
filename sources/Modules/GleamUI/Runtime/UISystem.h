#pragma once

#include "GleamPresentation/Runtime/PresentationSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"

#ifdef GleamRenderingRuntime
#include "GleamRendering/Runtime/System/RenderingSystem.h"
#define UISystemBase RelativeSystemGroupT<RenderingSystem,SystemOrderRelation::After>
#else
#define UISystemBase SystemGroupT<PresentationSystem>
#endif

namespace Gleam
{
    class UISystem : public UISystemBase
    {
        PresentationSystem* presentationSystem = nullptr;

        void Start() override;
        void Update() override;
    };
    Gleam_MakeSystem(UISystem)

    /**
     * UI子系统模板，便于快速编写自定义UI
     */
    class UISubSystem : public SystemT<UISystem>
    {
    };
}

#undef UISystemGroup