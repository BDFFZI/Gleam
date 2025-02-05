#include <iostream>
#include <gtest/gtest.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/System/TimeSystem.h"
#include "LightEngine/Runtime/System/UpdateSystem.h"

using namespace Light;

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
        if (countDown == 1)
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
Light_MakeSystem(MySystem)
Light_AddSystems(MySystem)