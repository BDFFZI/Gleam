#pragma once
#include <chrono>

#include "AwakeSystem.h"
#include "UpdateSystem.h"
#include "LightUtility/Runtime/Timer.h"

namespace Light
{
    class TimeSystem : public System
    {
    public:
        TimeSystem(): System(PreUpdateSystem)
        {
        }

        float GetTimeReal() const { return timeReal; }
        float GetTime() const { return time; }
        float GetDeltaTime() const { return deltaTime; }
        int GetFrameCount() const { return frameCount; }

        void SetTimeScale(const float scale) { timeScale = scale; }

    private:
        constexpr static float MaxDeltaTime = 0.1f;
        Timer<> timer;
        int frameCount = -1;
        float timeScale = 1;
        float timeReal = 0;
        float time = 0;
        float deltaTime = 0;

        void Update() override;
    };
    Light_MakeSystem(TimeSystem)
}