#include "TimeSystem.h"

namespace Gleam
{
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