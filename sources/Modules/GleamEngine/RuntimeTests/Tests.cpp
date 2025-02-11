#include <iostream>
#include <gtest/gtest.h>

#include "GleamECS/Runtime/World.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

using namespace Gleam;

class MySystem : public System
{
public:
    MySystem(): System(PostUpdateSystem)
    {
    }

    void Start() override
    {
        std::cout << "Engine Start" << std::endl;
    }
    void Update() override
    {
        countDown--;
        std::cout << "CountDown: " << countDown << std::endl;
        if (countDown == 0)
            Engine::Stop();

        std::cout
            << std::format("Time:{:f}\tDeltaTime:{:f}", TimeSystem->GetTime(), TimeSystem->GetDeltaTime())
            << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    void Stop() override
    {
        std::cout << "Engine Stop" << std::endl;
    }

    int countDown = 3;
};
Gleam_MakeSystemInstance(MySystem)
Gleam_AddSystems(MySystem)

Gleam_Main