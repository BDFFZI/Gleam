#pragma once
#include "UpdateSystem.h"
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Runtime/Entity/Hierarchy.h"
#include "GleamEngine/Runtime/Entity/Transform.h"

namespace Gleam
{
    class TransformSystem : public System<PostUpdateSystem>
    {
    public:
        static void ComputeLocalToWorld(LocalTransform localTransform, LocalToWorld& localToWorld);

    private:
        void Update() override;
    };
    Gleam_MakeSystem(TransformSystem)
}