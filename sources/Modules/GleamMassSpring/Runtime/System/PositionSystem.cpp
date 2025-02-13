#include "PositionSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"

void Gleam::PositionSystem::Update()
{
    //力->加速度->速度->位移
    View<Point, MassPointPhysics>::Each([](Point& point, MassPointPhysics& massPointPhysics)
    {
        //牛顿第二定律：加速度=力/质量
        float3 acceleration = massPointPhysics.force / massPointPhysics.mass;
        //计算速度（速度不会自然衰减，牛顿第一定律）
        massPointPhysics.velocity += acceleration * PhysicsSystem.GetFixedDeltaTime();
        //根据速度移动
        point.position += massPointPhysics.velocity * PhysicsSystem.GetFixedDeltaTime();
        //力已转化为速度
        massPointPhysics.force = 0;
    });
}