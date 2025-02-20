#include "CollisionSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMath/Runtime/Geometry/Geometry.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"
#include "GleamMath/Runtime/Geometry/3D/Cuboid.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Gleam
{
    template <class TShape>
        requires requires(TShape shape, float3 position)
        {
            Geometry::Contains(shape, position);
            Geometry::Extrudes(shape, position);
        }
    void ComputeCollision(
        LocalToWorld& localToWorld, WorldToLocal& worldToLocal, TShape shape, Collider collider,
        Point& point, MassPointPhysics& massPointPhysics, MassPointLastState& massPointLastState)
    {
        float3 localPosition = mul(worldToLocal.value, float4(point.position, 1)).xyz;
        if (Geometry::Contains(shape, localPosition) == false)
            return;
        float3 localOffset = Geometry::Extrudes(shape, localPosition) - localPosition;
        if (dot(localOffset, localOffset) < std::numeric_limits<float>::epsilon())
            return;

        float3 worldOffset = mul(static_cast<float3x3>(localToWorld.value), localOffset);
        //计算当前质点持有的力信息
        float3 deformationDirection = normalize(worldOffset);
        float3 movement = point.position - massPointLastState.lastPosition;
        float3 normalMovement = project(movement, deformationDirection);
        float3 tangentMovement = movement - normalMovement;
        //求解满足约束的质点位置
        float3 constrainedPosition = 0; //最合适的满足碰撞约束的质点位置
        float3 movementDirection = normalizesafe(movement);
        if (equal(dot(deformationDirection, movementDirection), 0.0f)) //初始在碰撞内或完全在表面滑动
            constrainedPosition = point.position + worldOffset;
        else //撞击碰撞体，回到质点撞击前所在的碰撞表面
            constrainedPosition = Geometry::Intersects(
                Plane{point.position + worldOffset, deformationDirection},
                Line{massPointLastState.lastPosition, movementDirection});

        //移动到初始约束位置并暂时清空力
        point.position = constrainedPosition;
        massPointLastState.lastPosition = point.position;
        //根据质点携带力和碰撞材质重新计算添加回去
        massPointLastState.lastPosition += normalMovement * collider.elasticity;
        point.position += tangentMovement * (1 - collider.friction);
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