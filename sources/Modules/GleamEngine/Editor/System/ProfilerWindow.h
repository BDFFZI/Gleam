#pragma once
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Editor/Profiler.h"

namespace Gleam
{
    class ProfilerWindow : public System
    {
    public:
        ProfilerWindow(): System(EditorUISystem)
        {
        }

        void SetProfile(Profile&& profiler);

    private:
        Profile lastProfile;
        Profile tickProfile;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(ProfilerWindow)
}