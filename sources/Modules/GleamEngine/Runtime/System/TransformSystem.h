#pragma once
#include "UpdateSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Runtime/Entity/Transform.h"

namespace Gleam
{
    class TransformSystem : public SystemT<PostUpdateSystem>
    {
    public:
        static void ComputeLocalToWorld(LocalTransform localTransform, LocalToWorld& localToWorld);

    private:
        void Update() override;
    };
    Gleam_MakeSystem(TransformSystem)
}