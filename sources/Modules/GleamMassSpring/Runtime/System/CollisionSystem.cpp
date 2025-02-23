#include "CollisionSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Component/Transform.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMath/Runtime/Geometry/Geometry.h"
#include "GleamMath/Runtime/Geometry/3D/Cuboid.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Editor/Gizmos.h"

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
        Particle& particle, int ccdMaxCount)
    {
        //基本碰撞检测
        float3 localPosition = mul(worldToLocal.value, float4(particle.position, 1)).xyz;
        float3 localCheckPoint = localPosition;
        bool isCollided = Geometry::Contains(shape, localCheckPoint);

        //ccd（连续碰撞检测）
        //由于碰撞约束的存在，在PositionSystem更新后，上次位置始终在碰撞体外，因为间隔期间只有碰撞约束会直接修改上次位置（人为除外）。
        //因此出现这种情况时，是因为质点同时陷入了多个碰撞体中。
        //而此时用ccd计算出来的点便会与其他碰撞约束冲突的，故只有上次位置不在碰撞内时才使用ccd。
        float3 localLastPosition = mul(worldToLocal.value, float4(particle.lastPosition, 1)).xyz;
        if (Geometry::Contains(shape, localLastPosition) == false)
        {
            for (int i = 1; i < ccdMaxCount; i++)
            {
                float3 newLocalCheckPoint = lerp(localCheckPoint, localLastPosition, 0.5f);
                if (Geometry::Contains(shape, newLocalCheckPoint))
                {
                    localCheckPoint = newLocalCheckPoint;
                    isCollided = true;
                }
            }
        }

        if (isCollided == false)
            return;

        //获取碰撞体表面“形变”信息
        float3 localOffset = Geometry::Extrudes(shape, localCheckPoint) - localCheckPoint;
        if (dot(localOffset, localOffset) < std::numeric_limits<float>::epsilon())
            return;
        float3 worldOffset = mul(static_cast<float3x3>(localToWorld.value), localOffset);
        float3 worldCheckPoint = mul(localToWorld.value, float4(localCheckPoint, 1)).xyz;

        //计算当前质点持有的力信息
        float3 deformationDirection = normalize(worldOffset);
        float3 movement = particle.position - particle.lastPosition;
        float3 normalMovement = project(movement, deformationDirection);
        float3 tangentMovement = movement - normalMovement;

        //求解满足约束的质点位置
        float3 constrainedPosition = 0; //最合适的满足碰撞约束的质点位置
        float3 movementDirection = normalizesafe(movement);
        if (std::abs(dot(deformationDirection, movementDirection)) < 0.1) //初始在碰撞内或完全在表面滑动
            constrainedPosition = worldCheckPoint + worldOffset;
        else //撞击碰撞体，回到质点撞击前所在的碰撞表面
            constrainedPosition = Geometry::Intersects(
                Plane{worldCheckPoint + worldOffset, deformationDirection},
                Line{particle.lastPosition, movementDirection});

        //移动到初始约束位置并暂时清空力（移动到约束位置的过程不需要记录为速度，力会在后续根据碰撞材质重新添加）
        //若要符合能量守恒还要额外位移来抵消嵌入的距离，但这容易导致其他约束的模拟效果不稳定。
        particle.position = constrainedPosition /*+ (constrainedPosition - particle.position)*/;
        particle.lastPosition = particle.position;

        //根据质点携带力和碰撞材质重新计算添加回去
        particle.lastPosition += normalMovement * collider.elasticity;
        ///摩擦力必须显式用位移实现。
        ///当位于碰撞表面的质点触发碰撞时，其所有位移一定是发生在碰撞体内（因为其在碰撞边界触发了碰撞），
        ///受碰撞位置约束影响，这些碰撞体内的位移将始终被碰撞约束全部撤回，于是质点就会卡在表面不动。
        ///而因为外力影响（如重力），无法保证力始终在碰撞表面的切线方向，因此受力引发的位移一定会出现上述情况。
        ///因此不能把摩擦力寄托于用力来实现位移，所以在此显式移动。
        particle.position += tangentMovement * (1 - collider.friction);
    }

    void CollisionSystem::Update()
    {
        View<LocalToWorld, WorldToLocal, Rectangle, Collider>::Each(
            [this](LocalToWorld& localToWorld, WorldToLocal& worldToLocal, Rectangle rectangle, Collider collider)
            {
                View<Particle>::Each(
                    [this, &localToWorld,&worldToLocal,&rectangle,&collider](Particle& particle)
                    {
                        ComputeCollision(localToWorld, worldToLocal, rectangle, collider, particle, ccdMaxCount);
                    }
                );
            }
        );

        View<LocalToWorld, WorldToLocal, Cuboid, Collider>::Each(
            [this](LocalToWorld& localToWorld, WorldToLocal& worldToLocal, Cuboid cuboid, Collider collider)
            {
                View<Particle>::Each(
                    [this,&localToWorld,&worldToLocal,&cuboid,&collider](Particle& particle)
                    {
                        ComputeCollision(localToWorld, worldToLocal, cuboid, collider, particle, ccdMaxCount);
                    }
                );
            }
        );

        View<LocalToWorld, WorldToLocal, Sphere, Collider>::Each(
            [this](LocalToWorld& localToWorld, WorldToLocal& worldToLocal, Sphere sphere, Collider collider)
            {
                View<Particle>::Each(
                    [this, &localToWorld,&worldToLocal,&sphere,&collider](Particle& particle)
                    {
                        ComputeCollision(localToWorld, worldToLocal, sphere, collider, particle, ccdMaxCount);
                    }
                );
            }
        );
    }
}