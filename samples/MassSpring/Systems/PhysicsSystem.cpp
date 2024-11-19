#include "PhysicsSystem.h"
#include "LightWindow/Runtime/Time.h"
#include "LightECS/Runtime/View.hpp"
#include "../Component.hpp"

void Light::PhysicsSystem::Update()
{
    float currentTime = Time::GetTime();
    float delta = currentTime - lastTime;
    while (delta >= DeltaTime)
    {
        UpdateMassPoint();
        UpdateSpring();
        delta -= DeltaTime;
        lastTime += DeltaTime;
    }

    UpdateRenderingData();
}

void Light::PhysicsSystem::UpdateSpring()
{
    // View<Transform, RigidBody, SpringPhysics>::Each([](Transform& transform, RigidBody& rigidBody, SpringPhysics& spring)
    // {
    //     float vector = spring.pinPosition - transform.position;
    //     float direction = vector >= 0 ? 1 : -1;
    //     float distance = abs(vector) - spring.length;
    //     float elasticForce = spring.elasticity * distance * direction; //弹力或推力
    //     float resistance = -0.01f * spring.elasticity * (rigidBody.velocity * direction) * direction; //弹簧内部阻力（不添加无法使弹簧稳定）
    //     rigidBody.force += elasticForce + resistance;
    // });
}
void Light::PhysicsSystem::UpdateMassPoint()
{
    View<Point, MassPointPhysics>::Each([](Point& point, MassPointPhysics& massPointPhysics)
    {
        //牛顿第二定律
        float2 acceleration = massPointPhysics.force / massPointPhysics.mass;
        massPointPhysics.force = 0;
        //添加重力加速度
        acceleration.y += -9.8f;
        //根据速度移动（速度不会自然衰减，牛顿第一定律）
        massPointPhysics.velocity += acceleration * DeltaTime;
        point.position += massPointPhysics.velocity * DeltaTime;
    });
}
void Light::PhysicsSystem::UpdateRenderingData()
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
