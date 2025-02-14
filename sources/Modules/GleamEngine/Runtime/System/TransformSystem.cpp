#include "TransformSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Runtime/Component/Hierarchy.h"
#include "GleamEngine/Runtime/Component/Transform.h"
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
        View<ViewExclusion<Parent>, LocalTransform, LocalToWorld>::Each(
            [this](LocalTransform& localTransform, LocalToWorld& localToWorld)
            {
                ComputeLocalToWorld(localTransform, localToWorld);
            }
        );

        View<LocalTransform, LocalToWorld, Parent>::Each([this](LocalTransform& local, LocalToWorld& localToWorld, Parent& parent)
        {
        });

        View<LocalToWorld, WorldToLocal>::Each([](LocalToWorld& localToWorld, WorldToLocal& worldToLocal)
        {
            worldToLocal.value = inverse(localToWorld.value);
        });
    }
}