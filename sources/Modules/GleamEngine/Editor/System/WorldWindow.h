#pragma once
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class WorldWindow : public System
    {
    public:
        WorldWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(WorldWindow)
}