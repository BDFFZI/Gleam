#include "TimeSystem.h"

namespace Gleam
{
    void TimeSystem::Update()
    {
        if (frameCount == -1)
            timer.Tick();

        const float currentTimeReal = static_cast<float>(static_cast<double>(timer.Time()) / 1000.0);
        deltaTimeReal = currentTimeReal - timeReal;
        timeReal = currentTimeReal;

        deltaTime = std::min(MaxDeltaTime, deltaTimeReal) * timeScale;
        time += deltaTime;

        frameCount++;
    }
}