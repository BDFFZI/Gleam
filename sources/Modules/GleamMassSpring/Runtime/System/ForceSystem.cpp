#include "ForceSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMassSpring/Runtime/Component/SpringPhysics.h"

void Gleam::ForceSystem::Update()
{
    //弹簧力
    View<SpringPhysics>::Each([](SpringPhysics& springPhysics)
    {
        Point* pointA;
        MassPointPhysics* massPointPhysicsA;
        MassPointLastState* massPointLastStateA;
        World::GetComponents(springPhysics.pointA, pointA, massPointPhysicsA, massPointLastStateA);
        Point* pointB;
        MassPointPhysics* massPointPhysicsB;
        MassPointLastState* massPointLastStateB;
        World::GetComponents(springPhysics.pointB, pointB, massPointPhysicsB, massPointLastStateB);

        //胡克定律：弹力=方向*弹力系数*距离
        float3 offset_BToA = pointA->position - pointB->position;
        float offsetSqrMagnitude_BToA = dot(offset_BToA, offset_BToA);
        if (offsetSqrMagnitude_BToA < std::numeric_limits<float>::epsilon())
            return;

        float offsetMagnitude_BToA = std::sqrt(offsetSqrMagnitude_BToA);
        float3 offsetDirection_BToA = offset_BToA / offsetMagnitude_BToA;
        float3 elasticity_BToA = offsetDirection_BToA * springPhysics.elasticity * (offsetMagnitude_BToA - springPhysics.length);
        massPointPhysicsB->force += elasticity_BToA;
        massPointPhysicsA->force -= elasticity_BToA;

        //阻力，衰竭一定的弹簧力以弥补积分误差（采用半隐式欧拉的方法计算，减少误差）
        float3 forceB = massPointLastStateB->lastForce + massPointPhysicsB->force;
        float3 forceA = massPointLastStateA->lastForce + massPointPhysicsA->force;
        float force_BToA = dot(forceB - forceA, offsetDirection_BToA); //使B向A接近的力总和
        float3 resistance_BToA = -offsetDirection_BToA * springPhysics.resistance * force_BToA;
        massPointPhysicsB->force += resistance_BToA;
        massPointPhysicsA->force -= resistance_BToA;
    });

    //重力
    View<MassPointPhysics>::Each([](MassPointPhysics& massPointPhysics)
    {
        massPointPhysics.force += PhysicsSystem.GetGravity() * massPointPhysics.mass;
    });
}