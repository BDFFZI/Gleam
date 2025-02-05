#include "PhysicsSystem.h"
#include "LightWindow/Runtime/Time.h"

void Light::PhysicsSystem::Update()
{
    float currentTime = Time->GetTime();
    float deltaTime = currentTime - lastTime;
    while (deltaTime >= fixedDeltaTime)
    {
        SystemGroup::Update();

        deltaTime -= fixedDeltaTime;
        lastTime += fixedDeltaTime;
    }
}
