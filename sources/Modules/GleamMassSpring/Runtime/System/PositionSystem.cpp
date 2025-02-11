#include "PositionSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMath/Runtime/Geometry/Point.h"

void Gleam::PositionSystem::Update()
{
    //力->加速度->速度->位移
    View<Point, MassPointPhysics>::Each([](Point& point, MassPointPhysics& massPointPhysics)
    {
        //计算加速度（牛顿第二定律）
        float3 acceleration = massPointPhysics.force / massPointPhysics.mass;
        massPointPhysics.force = 0;
        //计算速度（速度不会自然衰减，牛顿第一定律）
        massPointPhysics.velocity += acceleration * PhysicsSystem.GetFixedDeltaTime();
        massPointPhysics.velocity *= 1 - massPointPhysics.drag;//阻力降低速度
        //根据速度移动
        point.position += massPointPhysics.velocity * PhysicsSystem.GetFixedDeltaTime();
    });
}