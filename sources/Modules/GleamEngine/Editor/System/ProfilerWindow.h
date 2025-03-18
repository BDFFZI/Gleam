#pragma once
#include "EditorUISystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Editor/Profiler.h"

namespace Gleam
{
    class ProfilerWindow : public System
    {
    public:
        ProfilerWindow(): System(GlobalEditorUISystem)
        {
        }

        void SetProfile(Profile&& profiler);

    private:
        Profile lastProfile;
        Profile tickProfile;

        void Update() override;
    };
    Gleam_MakeGlobalSystem(ProfilerWindow)

    void ProfilerWindow_FetchProfile();
}