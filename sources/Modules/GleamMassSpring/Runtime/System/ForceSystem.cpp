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

        float3 elasticityVector_BToA = pointA->position - pointB->position;
        float3 elasticityMagnitude_BToA = length(elasticityVector_BToA);
        float3 elasticityDirection_BToA = elasticityVector_BToA / elasticityMagnitude_BToA;
        //胡克定律：弹力=弹力系数*方向*距离
        float3 elasticityBToA = springPhysics.elasticity * elasticityDirection_BToA * (elasticityMagnitude_BToA - springPhysics.length);

        float3 velocity_BToA = massPointPhysicsA->velocity - massPointPhysicsB->velocity;
        float3 resistance_BToA = -springPhysics.resistance * elasticityDirection_BToA * dot(-velocity_BToA, elasticityDirection_BToA);

        massPointPhysicsB->force += elasticityBToA + resistance_BToA;
        massPointPhysicsA->force -= elasticityBToA + resistance_BToA;
    });

    //重力
    View<MassPointPhysics>::Each([](MassPointPhysics& massPointPhysics)
    {
        massPointPhysics.force += PhysicsSystem.GetGravity() * massPointPhysics.mass;
    });

    //空气阻力
}