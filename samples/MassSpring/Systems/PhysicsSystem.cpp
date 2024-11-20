#include "PhysicsSystem.h"
#include "LightWindow/Runtime/Time.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"

void Light::EndPhysicsSystem::Update()
{
    float currentTime = Time::GetTime();
    float deltaTime = currentTime - Physics::lastTime;
    while (deltaTime >= Physics::fixedDeltaTime)
    {
        CollectForce();
        for (const auto fixedUpdate : Physics::fixedUpdates)
            fixedUpdate();
        ResolveForce();

        deltaTime -= Physics::fixedDeltaTime;
        Physics::lastTime += Physics::fixedDeltaTime;
    }

    UpdateRenderingData();
}

void Light::EndPhysicsSystem::CollectForce()
{
    //弹力
    View<SpringPhysics>::Each([](SpringPhysics& springPhysics)
    {
        Point pointA;
        MassPointPhysics massPointPhysicsA;
        World::GetComponents(springPhysics.pointA, &pointA, &massPointPhysicsA);
        Point pointB;
        MassPointPhysics massPointPhysicsB;
        World::GetComponents(springPhysics.pointB, &pointB, &massPointPhysicsB);
    });

    //重力
    View<MassPointPhysics>::Each([](MassPointPhysics& massPointPhysics)
    {
        massPointPhysics.force += Physics::gravity * massPointPhysics.mass;
    });
}
void Light::EndPhysicsSystem::ResolveForce()
{
    //力->加速度->速度->位移
    View<Point, MassPointPhysics>::Each([](Point& point, MassPointPhysics& massPointPhysics)
    {
        //牛顿第二定律
        float2 acceleration = massPointPhysics.force / massPointPhysics.mass;
        massPointPhysics.force = 0;
        //根据速度移动（速度不会自然衰减，牛顿第一定律）
        massPointPhysics.velocity += acceleration * Physics::fixedDeltaTime;
        point.position += massPointPhysics.velocity * Physics::fixedDeltaTime;
    });
}

void Light::EndPhysicsSystem::UpdateRenderingData()
{
    View<Line, SpringPhysics>::Each([](Line& line, SpringPhysics& springPhysics)
    {
        Point pointA;
        MassPointPhysics massPointPhysicsA;
        World::GetComponents(springPhysics.pointA, &pointA, &massPointPhysicsA);
        Point pointB;
        MassPointPhysics massPointPhysicsB;
        World::GetComponents(springPhysics.pointB, &pointB, &massPointPhysicsB);

        line.positionA = pointA.position;
        line.positionB = pointB.position;
    });
}
