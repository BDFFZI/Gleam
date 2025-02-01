#include "TransformSystem.h"

#include "LightECS/Runtime/View.h"
#include "LightEngine/Runtime/Component/Hierarchy.h"
#include "LightEngine/Runtime/Component/Transform.h"
#include "LightMath/Runtime/LinearAlgebra/MatrixMath.h"

namespace Light
{
    void TransformSystem::Update()
    {
        View<ViewExclusion<Parent>, LocalTransform, LocalToWorld>::Each(
            [this](LocalTransform& local, LocalToWorld& localToWorld)
            {
                float3x3 scale = float3x3::Scale(local.scale);
                float3x3 rotate = local.rotation.ToRotationMatrix();
                float4x4 translate = float4x4::Translate(local.position);
                localToWorld.value = mul(translate, static_cast<float4x4>(mul(rotate, scale)));
            }
        );

        View<LocalTransform, LocalToWorld, Parent>::Each([this](LocalTransform& local, LocalToWorld& localToWorld, Parent& parent)
        {
        });
    }
}