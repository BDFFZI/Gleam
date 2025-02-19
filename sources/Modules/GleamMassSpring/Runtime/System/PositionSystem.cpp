#include "PositionSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"

void Gleam::PositionSystem::Update()
{
    View<Point, MassPointPhysics, MassPointLastState>::Each([](Point& point, MassPointPhysics& massPointPhysics, MassPointLastState& massPointLastState)
    {
        //牛顿第二定律：加速度=力/质量
        float3 acceleration = massPointPhysics.force / massPointPhysics.mass;
        massPointPhysics.force = 0;
        //Verlet积分法
        float3 lastPosition = massPointLastState.lastPosition;
        massPointLastState.lastPosition = point.position;
        point.position = point.position + (
            point.position - lastPosition + //位移
            acceleration * PhysicsSystem.GetFixedDeltaTime() * PhysicsSystem.GetFixedDeltaTime() //加速度
        ) * (1 - massPointPhysics.drag); //始终存在误差问题，所以要用阻力修正

        float3 velocity = (point.position - lastPosition) / (2 * PhysicsSystem.GetFixedDeltaTime());
        acceleration = velocity / PhysicsSystem.GetFixedDeltaTime() + acceleration;
        massPointLastState.lastForce = acceleration * massPointPhysics.mass * (1 - massPointPhysics.drag);
    });
}