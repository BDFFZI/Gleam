#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/System/EditorUISystem.h"

namespace Gleam
{
    class ProjectWindow : public System
    {
    public:
        ProjectWindow(): System(GlobalEditorUISystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(ProjectWindow)
}