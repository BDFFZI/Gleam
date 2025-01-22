#include "ForceSystem.h"

#include "PhysicsComponent.h"
#include "LightECS/Runtime/View.h"
#include "LightMath/Runtime/VectorMath.h"
#include "../Public/Component.h"

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
        massPointPhysics.force += PhysicsSystem.GetGravity() * massPointPhysics.mass;
    });
}
