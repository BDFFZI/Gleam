#include "CollisionSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Gleam
{
    void ComputeCollision(Point& point, MassPointPhysics& massPointPhysics, Collider& collider, float3 offset)
    {
        float3 normal = normalize(offset);
        float3 normalForce = project(massPointPhysics.lastForce, normal);
        float3 tangentForce = massPointPhysics.lastForce - normalForce;

        massPointPhysics.lastForce +=
            -normalForce * (1 + collider.elasticity) //反作用力
            - tangentForce * collider.friction; //摩擦力
        point.position += offset;
    }

    void CollisionSystem::Update()
    {
        View<LocalToWorld, WorldToLocal, Cuboid, Collider>::Each(
            [](LocalToWorld& localToWorld, WorldToLocal& worldToLocal, Cuboid cuboid, Collider collider)
            {
                View<Point, MassPointPhysics>::Each(
                    [&localToWorld,&worldToLocal,&cuboid,&collider](Point& point, MassPointPhysics& massPointPhysics)
                    {
                        float3 localPosition = mul(worldToLocal.value, float4(point.position, 1)).xyz;
                        float3 localOffset = cuboid.Extrudes(localPosition) - localPosition;
                        float3 worldOffset = mul(static_cast<float3x3>(localToWorld.value), localOffset);
                        ComputeCollision(point, massPointPhysics, collider, worldOffset);
                    }
                );
            }
        );

        View<LocalToWorld, WorldToLocal, Sphere, Collider>::Each(
            [](LocalToWorld& localToWorld, WorldToLocal& worldToLocal, Sphere sphere, Collider collider)
            {
                View<Point, MassPointPhysics>::Each(
                    [&localToWorld,&worldToLocal,&sphere,&collider](Point& point, MassPointPhysics& massPointPhysics)
                    {
                        float3 localPosition = mul(worldToLocal.value, float4(point.position, 1)).xyz;
                        float3 localOffset = sphere.Extrudes(localPosition) - localPosition;
                        float3 worldOffset = mul(static_cast<float3x3>(localToWorld.value), localOffset);
                        ComputeCollision(point, massPointPhysics, collider, worldOffset);
                    }
                );
            }
        );
    }
}