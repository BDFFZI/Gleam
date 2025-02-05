#include "ForceSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"
#include "LightMassSpring/Runtime/Component/SpringPhysics.h"

void Light::ForceSystem::Update()
{
    //弹力
    View<SpringPhysics>::Each([](SpringPhysics& springPhysics)
    {
        Point* pointA;
        MassPointPhysics* massPointPhysicsA;
        springPhysics.pointA.Get(&pointA, &massPointPhysicsA);
        Point* pointB;
        MassPointPhysics* massPointPhysicsB;
        springPhysics.pointB.Get(&pointB, &massPointPhysicsB);

        float3 elasticityVector_BToA = pointA->position - pointB->position;
        float3 elasticityDirection_BToA = normalize(elasticityVector_BToA);
        float3 elasticityMagnitude_BToA = length(elasticityVector_BToA);
        float3 elasticityBToA = springPhysics.elasticity * elasticityDirection_BToA * (elasticityMagnitude_BToA - springPhysics.length);

        float3 velocity_BToA = massPointPhysicsA->velocity - massPointPhysicsB->velocity;
        float3 resistance_BToA = -springPhysics.resistance * elasticityDirection_BToA * dot(-velocity_BToA, elasticityDirection_BToA);

        massPointPhysicsB->force += elasticityBToA + resistance_BToA;
        massPointPhysicsA->force -= elasticityBToA + resistance_BToA;
    });

    //重力
    View<MassPointPhysics>::Each([](MassPointPhysics& massPointPhysics)
    {
        massPointPhysics.force += PhysicsSystem->GetGravity() * massPointPhysics.mass;
    });
}
