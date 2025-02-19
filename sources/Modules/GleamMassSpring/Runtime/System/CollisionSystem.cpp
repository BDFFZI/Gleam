#include "CollisionSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"
#include "GleamMath/Runtime/Geometry/Solid/Cuboid.h"
#include "GleamMath/Runtime/Geometry/Solid/Sphere.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Editor/Gizmos.h"

namespace Gleam
{
    template <class TShape>
        requires requires(TShape shape, float3 position)
        {
            shape.Contains(position);
            shape.Extrudes(position);
        }
    void ComputeCollision(
        LocalToWorld& localToWorld, WorldToLocal& worldToLocal, TShape shape, Collider collider,
        Point& point, MassPointPhysics& massPointPhysics, MassPointLastState& massPointLastState)
    {
        float3 localPosition = mul(worldToLocal.value, float4(point.position, 1)).xyz;
        if (shape.Contains(localPosition) == false)
            return;
        float3 localOffset = shape.Extrudes(localPosition) - localPosition;
        if (dot(localOffset, localOffset) < std::numeric_limits<float>::epsilon())
            return;

        float3 worldOffset = mul(static_cast<float3x3>(localToWorld.value), localOffset);
        float3 normal = normalize(worldOffset);
        float3 normalForce = project(massPointLastState.lastForce, normal);
        float3 tangentForce = massPointLastState.lastForce - normalForce;

        ///抵消质点施加的力，并位移到表面。
        ///位移到表面导致每次弹回时都有额外的偏移，因此力不平衡，当弹力为1时会越弹越大，而不是保持高度。
        ///但不位移到表面会导致，不满足碰撞约束，且质点抖动或下陷。因为虽然力平衡了，但碰撞前质点已被位移。
        point.position += worldOffset;
        massPointLastState.lastPosition = point.position;
        //其他反作用力
        massPointPhysics.force += -normalForce * collider.elasticity; //弹力
        massPointPhysics.force += -tangentForce * collider.friction; //摩擦力
    }

    void CollisionSystem::Update()
    {
        View<LocalToWorld, WorldToLocal, Cuboid, Collider>::Each(
            [](LocalToWorld& localToWorld, WorldToLocal& worldToLocal, Cuboid cuboid, Collider collider)
            {
                View<Point, MassPointPhysics, MassPointLastState>::Each(
                    [&localToWorld,&worldToLocal,&cuboid,&collider](Point& point, MassPointPhysics& massPointPhysics, MassPointLastState& massPointLastState)
                    {
                        ComputeCollision(localToWorld, worldToLocal, cuboid, collider, point, massPointPhysics, massPointLastState);
                    }
                );
            }
        );

        View<LocalToWorld, WorldToLocal, Sphere, Collider>::Each(
            [](LocalToWorld& localToWorld, WorldToLocal& worldToLocal, Sphere sphere, Collider collider)
            {
                View<Point, MassPointPhysics, MassPointLastState>::Each(
                    [&localToWorld,&worldToLocal,&sphere,&collider](Point& point, MassPointPhysics& massPointPhysics, MassPointLastState& massPointLastState)
                    {
                        ComputeCollision(localToWorld, worldToLocal, sphere, collider, point, massPointPhysics, massPointLastState);
                    }
                );
            }
        );
    }
}