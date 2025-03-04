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
        TimeSystem(): System(GlobalPreUpdateSystem)
        {
        }

        float GetMaxDeltaTime() const { return maxDeltaTime; }
        float GetFixedDeltaTime() const { return fixedDeltaTime; }
        float GetTimeScale() const { return timeScale; }
        bool GetAutoStepTime() const { return autoStepTime; }

        int GetFrameCount() const { return frameCount; }
        float GetTimeReal() const { return timeReal; }
        float GetDeltaTimeReal() const { return deltaTimeReal; }
        float GetTime() const { return time; }
        float GetDeltaTime() const { return deltaTime; }
        int GetFixedDeltaCount() const { return fixedDeltaCount; }

        void SetTimeScale(const float scale) { timeScale = scale; }
        void SetAutoStepTime(bool state);
        void SetStepTime(float stepTime);

    private:
        Gleam_MakeType_Friend

        Timer<> timer;
        float maxDeltaTime = 0.1f;
        float fixedDeltaTime = 0.01f;
        float timeScale = 1;
        bool autoStepTime = true;
        float stepTime = 0;

        int frameCount = -1;
        float deltaTimeReal = 0;
        float timeReal = 0;
        float deltaTime = 0;
        float time = 0;
        float fixedTime = 0;
        int fixedDeltaCount = 0;

        void Start() override;
        void Update() override;
    };

    Gleam_MakeType(TimeSystem, "")
    {
        Gleam_MakeType_AddField(maxDeltaTime);
        Gleam_MakeType_AddField(fixedDeltaTime);
        Gleam_MakeType_AddField(timeScale);
        Gleam_MakeType_AddField(autoStepTime);

        Gleam_MakeType_AddField(frameCount);
        Gleam_MakeType_AddField(deltaTimeReal);
        Gleam_MakeType_AddField(timeReal);
        Gleam_MakeType_AddField(deltaTime);
        Gleam_MakeType_AddField(time);
        Gleam_MakeType_AddField(fixedTime);
        Gleam_MakeType_AddField(fixedDeltaCount);
    }

    Gleam_MakeGlobalSystem(TimeSystem)
}