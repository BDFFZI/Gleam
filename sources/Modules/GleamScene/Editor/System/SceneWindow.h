#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"

namespace Gleam
{
    class SceneWindow : public System
    {
    public:
        SceneWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(SceneWindow)
}