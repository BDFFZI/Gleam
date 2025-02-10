#pragma once
#include <chrono>

#include "UpdateSystem.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamUtility/Runtime/Timer.h"

namespace Gleam
{
    class TimeSystem : public System
    {
    public:
        TimeSystem(const std::string_view name = ""): System(PreUpdateSystem)
        {
            GetName() = name;
        }

        float GetTimeReal() const { return timeReal; }
        float GetTime() const { return time; }
        float GetDeltaTimeReal() const { return deltaTimeReal; }
        float GetDeltaTime() const { return deltaTime; }
        int GetFrameCount() const { return frameCount; }

        void SetTimeScale(const float scale) { timeScale = scale; }

    private:
        Gleam_MakeType_Friend

        constexpr static float MaxDeltaTime = 0.1f;
        Timer<> timer;
        int frameCount = -1;
        float deltaTimeReal = 0;
        float timeReal = 0;
        float timeScale = 1;
        float deltaTime = 0;
        float time = 0;

        void Update() override;
    };
    Gleam_MakeSystemInstance(TimeSystem)

    Gleam_MakeType(TimeSystem_T, "")
    {
        Gleam_MakeType_AddField(frameCount);
        Gleam_MakeType_AddField(deltaTimeReal);
        Gleam_MakeType_AddField(timeReal);
        Gleam_MakeType_AddField(timeScale);
        Gleam_MakeType_AddField(deltaTime);
        Gleam_MakeType_AddField(time);
    }
}