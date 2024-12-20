#include <iostream>
#include <thread>
#include <LightEngine/Runtime/Engine.h>

#include "LightECS/Runtime/System.h"
#include "LightECS/Runtime/World.h"

using namespace Light;

class CustomSystem : public System
{
public:
    CustomSystem(): System(nullptr)
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
    }
    void Stop() override
    {
        std::cout << "Engine Stop" << std::endl;
    }

private:
    int countDown = 3;
};
Light_MakeSystem(CustomSystem)
Light_AddSystem(CustomSystem)

int main()
{
    Engine::Start();
    return 0;
}