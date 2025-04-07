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
        Profile lastProfile; //上一帧的探针信息
        Profile tickProfile; //抓取的探针信息
        Profile showProfile; //显示中的探针信息（可能被修改，因此需和抓取的原始信息分开）
        bool sort = false;

        void SetShowProfile(Profile& profile);
        void Update() override;
    };
    Gleam_MakeSystem(ProfilerWindow)

    void ProfilerWindow_FetchProfile();
}