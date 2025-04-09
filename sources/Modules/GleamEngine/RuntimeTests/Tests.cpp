#include <iostream>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/World/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

using namespace Gleam;

// class GameSystem

class MySystem : public SystemT<PostUpdateSystem>
{
    TimeSystem* timeSystem = nullptr;
    int countDown = 3;

    void Start() override
    {
        timeSystem = World::GetCurrentWorld().GetSystemAllocator().GetSystemPtr<TimeSystem>().lock().get();

        std::cout << "Engine Start" << std::endl;
    }
    void Update() override
    {
        countDown--;
        std::cout << "CountDown: " << countDown << std::endl;
        if (countDown == 0)
            Engine::Stop();

        Time& time = timeSystem->GetMainTime();
        std::cout
            << std::format("Time:{:f}\tDeltaTime:{:f}", time.GetTime(), time.GetDeltaTime())
            << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    void Stop() override
    {
        std::cout << "Engine Stop" << std::endl;
    }
};
Gleam_MakeSystem(MySystem)
Gleam_AddRuntimeSystems(MySystem)

Gleam_Main