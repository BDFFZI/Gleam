#include "TransformSystem.h"

#include "GleamEngine/Runtime/Entity/Hierarchy.h"
#include "GleamEngine/Runtime/Entity/Transform.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Gleam
{
    void TransformSystem::ComputeLocalToWorld(LocalTransform localTransform, LocalToWorld& localToWorld)
    {
        float3x3 scale = float3x3::Scale(localTransform.scale);
        float3x3 rotate = localTransform.rotation.ToRotationMatrix();
        float4x4 translate = float4x4::Translate(localTransform.position);
        localToWorld.value = mul(translate, static_cast<float4x4>(mul(rotate, scale)));
    }

    void TransformSystem::Update()
    {
        World::View<QueryExclusion<Parent>, LocalTransform, LocalToWorld>().Each([](LocalTransform& localTransform, LocalToWorld& localToWorld)
        {
            ComputeLocalToWorld(localTransform, localToWorld);
        });

        World::View<LocalTransform, LocalToWorld, Parent>().Each([](LocalTransform& local, LocalToWorld& localToWorld, Parent& parent)
        {
        });

        World::View<LocalToWorld, WorldToLocal>().Each([](LocalToWorld& localToWorld, WorldToLocal& worldToLocal)
        {
            worldToLocal.value = inverse(localToWorld.value);
        });
    }
}