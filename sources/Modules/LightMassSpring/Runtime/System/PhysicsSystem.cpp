#include "PhysicsSystem.h"

#include "LightEngine/Runtime/System/TimeSystem.h"

void Light::PhysicsSystem::Update()
{
    float currentTime = TimeSystem->GetTime();
    float deltaTime = currentTime - lastTime;
    while (deltaTime >= fixedDeltaTime)
    {
        SystemGroup::Update();

        deltaTime -= fixedDeltaTime;
        lastTime += fixedDeltaTime;
    }
}
