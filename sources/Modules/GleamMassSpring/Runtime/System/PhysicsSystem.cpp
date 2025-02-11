#include "PhysicsSystem.h"

#include "GleamEngine/Runtime/System/TimeSystem.h"

void Gleam::PhysicsSystem::Update()
{
    float currentTime = TimeSystem.GetTime();
    float deltaTime = currentTime - lastTime;
    while (deltaTime >= fixedDeltaTime)
    {
        SystemGroup::Update();

        deltaTime -= fixedDeltaTime;
        lastTime += fixedDeltaTime;
    }
}
