#pragma once
#include "Window.h"
#include "LightEngine/Runtime/ECS/System.h"

namespace Light
{
    class Time : public System
    {
    public:
        Time(): System(Window, LeftOrder, MiddleOrder)
        {
        }

        float GetTimeReal() const { return timeReal; }
        float GetTime() const { return time; }
        float GetDeltaTime() const { return deltaTime; }
        int GetFrameCount() const { return frameCount; }

        void SetTimeScale(const float scale) { timeScale = scale; }

    private:
        int frameCount = -1;
        float timeScale = 1;
        float timeReal = 0;
        float time = 0;
        float deltaTime = 0;

        void Start() override;
        void Update() override;
    };
    Light_MakeSystem(Time)
}