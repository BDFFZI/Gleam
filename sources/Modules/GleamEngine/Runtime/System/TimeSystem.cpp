#include "TimeSystem.h"

#include "GleamEngine/Runtime/Entity/Time.h"

namespace Gleam
{
    void TimeSystem::Update()
    {
        view.Each([](Time& time)
        {
            if (time.frameCount == -1)
                time.timer.Tick();

            //帧数
            time.frameCount++;
            //真实时间
            const float currentTimeReal = static_cast<float>(static_cast<double>(time.timer.Time()) / 1000.0);
            time.deltaTimeReal = currentTimeReal - time.timeReal;
            time.timeReal = currentTimeReal;
            //游戏时间
            if (time.autoStepTime)
                time.deltaTime = std::min(time.maxDeltaTime, time.deltaTimeReal) * time.timeScale;
            else
            {
                time.deltaTime = time.stepTime;
                time.stepTime = 0;
            }
            time.time += time.deltaTime;
            //固定更新游戏时间
            float fixedTimeDelta = time.time - time.fixedTime;
            time.fixedDeltaCount = static_cast<int>(fixedTimeDelta / time.fixedDeltaTime);
            time.fixedTime += static_cast<float>(time.fixedDeltaCount) * time.fixedDeltaTime;
        });
    }
}