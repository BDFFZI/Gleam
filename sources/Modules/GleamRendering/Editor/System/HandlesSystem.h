#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"
#include "GleamUI/Runtime/UISystem.h"

namespace Gleam
{
    class HandlesSystem : public System
    {
    public:
        HandlesSystem(): System(GlobalEditorUISystem, MinOrder, DefaultOrder)
        {
        }

    private:
        void Start() override;
        void Update() override;
    };
    Gleam_MakeGlobalSystem(HandlesSystem)
}