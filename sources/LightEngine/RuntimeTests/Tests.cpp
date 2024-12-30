#include <iostream>
#include <gtest/gtest.h>

#include "LightEngine/Runtime/Engine.h"
#include "LightEngine/Runtime/ECS/System.h"

using namespace Light;

class MySystem : public System
{
public:
    MySystem(): System(SimulationSystem)
    {
    }

    void Start() override
    {
        std::cout << "Engine Start" << std::endl;
    }
    void Update() override
    {
        myData++;
    }
    void Stop() override
    {
        std::cout << "Engine Stop" << std::endl;
    }

private:
    int myData = 0;
};
Light_MakeSystem(MySystem)
Light_AddSystems(MySystem)

TEST(Engine, Start)
{
    Engine::Start();
}