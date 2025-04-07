#pragma once
#include "GleamECS/Runtime/System/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"

namespace Gleam
{
    class AnimationWindow : public System
    {
    public:
        AnimationWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeSystem(AnimationWindow);
}