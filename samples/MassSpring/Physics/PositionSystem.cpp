#include "PositionSystem.h"
#include "PhysicsComponent.h"
#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"
#include "LightECS/Runtime/View.h"

void Light::PositionSystem::Update()
{
    //力->加速度->速度->位移
    View<Point, MassPointPhysics>::Each([](Point& point, MassPointPhysics& massPointPhysics)
    {
        //计算加速度（牛顿第二定律）
        float2 acceleration = massPointPhysics.force / massPointPhysics.mass;
        massPointPhysics.force = 0;
        //计算速度（速度不会自然衰减，牛顿第一定律）
        massPointPhysics.velocity += acceleration * PhysicsSystem.GetFixedDeltaTime();
        massPointPhysics.velocity *= massPointPhysics.drag;
        //根据速度移动
        point.position += massPointPhysics.velocity * PhysicsSystem.GetFixedDeltaTime();
    });
}
