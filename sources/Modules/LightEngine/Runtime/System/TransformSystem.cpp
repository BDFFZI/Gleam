#include "TransformSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightEngine/Runtime/Component/Hierarchy.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Light
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
    }
}