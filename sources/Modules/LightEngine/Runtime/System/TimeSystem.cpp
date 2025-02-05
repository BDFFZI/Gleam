#include "TimeSystem.h"

namespace Light
{
    void TimeSystem::Start()
    {
        timer.Tick();
    }
    void TimeSystem::Update()
    {
        const float currentTimeReal = static_cast<float>(static_cast<double>(timer.Time()) / 1000.0);
        deltaTime = currentTimeReal - timeReal;
        timeReal = currentTimeReal;

        deltaTime *= timeScale;
        time += deltaTime;

        frameCount++;
    }
}