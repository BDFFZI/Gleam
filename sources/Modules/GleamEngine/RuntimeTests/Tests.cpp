#include <iostream>
#include <gtest/gtest.h>

#include "GleamEngine/Runtime/Engine.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

using namespace Gleam;

class MySystem : public System<PostUpdateSystem>
{
    int countDown = 3;

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
            << std::format("Time:{:f}\tDeltaTime:{:f}", GlobalTimeSystem->GetTimeReal(), GlobalTimeSystem->GetDeltaTimeReal())
            << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    void Stop() override
    {
        std::cout << "Engine Stop" << std::endl;
    }
};
Gleam_MakeRuntimeSystem(MySystem)

Gleam_Main