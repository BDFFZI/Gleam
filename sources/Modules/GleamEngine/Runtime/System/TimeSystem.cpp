#include "TimeSystem.h"

namespace Gleam
{
    void TimeSystem::SetAutoStepTime(const bool state)
    {
        autoStepTime = state;
    }
    void TimeSystem::SetStepTime(const float stepTime)
    {
        this->stepTime = stepTime;
    }

    void TimeSystem::Start()
    {
        maxDeltaTime = 0.1f;
        fixedDeltaTime = 0.01f;
        timeScale = 1;
        stepTime = 0;

        frameCount = -1;
        deltaTimeReal = 0;
        timeReal = 0;
        deltaTime = 0;
        time = 0;
        fixedTime = 0;
        fixedDeltaCount = 0;
    }
    void TimeSystem::Update()
    {
        if (frameCount == -1)
            timer.Tick();

        //帧数
        frameCount++;
        //真实时间
        const float currentTimeReal = static_cast<float>(static_cast<double>(timer.Time()) / 1000.0);
        deltaTimeReal = currentTimeReal - timeReal;
        timeReal = currentTimeReal;
        //游戏时间
        if (autoStepTime)
            deltaTime = std::min(maxDeltaTime, deltaTimeReal) * timeScale;
        else
        {
            deltaTime = stepTime;
            stepTime = 0;
        }
        time += deltaTime;
        //固定更新游戏时间
        float fixedTimeDelta = time - fixedTime;
        fixedDeltaCount = static_cast<int>(fixedTimeDelta / fixedDeltaTime);
        fixedTime += static_cast<float>(fixedDeltaCount) * fixedDeltaTime;
    }
}