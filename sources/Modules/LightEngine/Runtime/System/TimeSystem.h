#pragma once
#include <chrono>

#include "UpdateSystem.h"
#include "LightReflection/Runtime/Type.h"
#include "LightUtility/Runtime/Timer.h"

namespace Light
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
        Light_MakeType_Friend

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
    Light_MakeSystemInstance(TimeSystem)

    Light_MakeType(TimeSystem_T, "")
    {
        Light_MakeType_AddField(frameCount);
        Light_MakeType_AddField(deltaTimeReal);
        Light_MakeType_AddField(timeReal);
        Light_MakeType_AddField(timeScale);
        Light_MakeType_AddField(deltaTime);
        Light_MakeType_AddField(time);
    }
}