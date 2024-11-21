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
        Point* pointA;
        MassPointPhysics* massPointPhysicsA;
        World::GetComponents(springPhysics.pointA, &pointA, &massPointPhysicsA);
        Point* pointB;
        MassPointPhysics* massPointPhysicsB;
        World::GetComponents(springPhysics.pointB, &pointB, &massPointPhysicsB);

        float2 elasticityVector_BToA = pointA->position - pointB->position;
        float2 elasticityDirection_BToA = normalize(elasticityVector_BToA);
        float2 elasticityMagnitude_BToA = length(elasticityVector_BToA);
        float2 elasticityBToA = springPhysics.elasticity * elasticityDirection_BToA * (elasticityMagnitude_BToA - springPhysics.length);

        float2 velocity_BToA = massPointPhysicsA->velocity - massPointPhysicsB->velocity;
        float2 resistance_BToA = -springPhysics.resistance * elasticityDirection_BToA * dot(-velocity_BToA, elasticityDirection_BToA);

        massPointPhysicsB->force += elasticityBToA + resistance_BToA;
        massPointPhysicsA->force -= elasticityBToA + resistance_BToA;
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
        //计算加速度（牛顿第二定律）
        float2 acceleration = massPointPhysics.force / massPointPhysics.mass;
        massPointPhysics.force = 0;
        //计算速度（速度不会自然衰减，牛顿第一定律）
        massPointPhysics.velocity += acceleration * Physics::fixedDeltaTime;
        massPointPhysics.velocity *= massPointPhysics.drag;
        //根据速度移动
        point.position += massPointPhysics.velocity * Physics::fixedDeltaTime;
    });
}

void Light::EndPhysicsSystem::UpdateRenderingData()
{
    View<Line, SpringPhysics>::Each([](Line& line, SpringPhysics& springPhysics)
    {
        Point pointA;
        MassPointPhysics massPointPhysicsA;
        World::CopyComponents(springPhysics.pointA, &pointA, &massPointPhysicsA);
        Point pointB;
        MassPointPhysics massPointPhysicsB;
        World::CopyComponents(springPhysics.pointB, &pointB, &massPointPhysicsB);

        line.positionA = pointA.position;
        line.positionB = pointB.position;
    });
}
