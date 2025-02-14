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
        springPhysics.pointA.Get(pointA, massPointPhysicsA);
        Point* pointB;
        MassPointPhysics* massPointPhysicsB;
        springPhysics.pointB.Get(pointB, massPointPhysicsB);

        //胡克定律：弹力=方向*弹力系数*距离
        float3 offset_BToA = pointA->position - pointB->position;
        float offsetMagnitude_BToA = length(offset_BToA);
        float3 offsetDirection_BToA = offset_BToA / offsetMagnitude_BToA;
        float elasticity_BToA = springPhysics.elasticity * (offsetMagnitude_BToA - springPhysics.length);
        float3 elasticityVector_BToA = offsetDirection_BToA * elasticity_BToA;

        //阻力，衰竭一定的弹簧力以弥补积分误差
        float3 forceB = massPointPhysicsB->lastForce + massPointPhysicsB->force;
        float3 forceA = massPointPhysicsA->lastForce + massPointPhysicsA->force;
        float forceBToA = dot(forceB - forceA, offsetDirection_BToA); //使B向A接近的力总和
        float3 resistanceBToA = -offsetDirection_BToA * springPhysics.resistance * forceBToA;

        massPointPhysicsB->force += elasticityVector_BToA + resistanceBToA;
        massPointPhysicsA->force -= elasticityVector_BToA + resistanceBToA;
    });

    //重力
    View<MassPointPhysics>::Each([](MassPointPhysics& massPointPhysics)
    {
        massPointPhysics.force += PhysicsSystem.GetGravity() * massPointPhysics.mass;
    });
}