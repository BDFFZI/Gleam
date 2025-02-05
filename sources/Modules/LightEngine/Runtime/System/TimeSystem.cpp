#include "TimeSystem.h"

namespace Light
{
    void TimeSystem::Update()
    {
        if (frameCount == -1)
            timer.Tick();

        const float currentTimeReal = static_cast<float>(static_cast<double>(timer.Time()) / 1000.0);
        deltaTime = std::min(MaxDeltaTime, currentTimeReal - timeReal);
        timeReal = currentTimeReal;

        deltaTime *= timeScale;
        time += deltaTime;

        frameCount++;
    }
}