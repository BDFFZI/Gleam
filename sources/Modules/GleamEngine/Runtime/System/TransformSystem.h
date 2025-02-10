#pragma once
#include "UpdateSystem.h"
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/Component/Transform.h"

namespace Gleam
{
    class TransformSystem : public System
    {
    public:
        static void ComputeLocalToWorld(LocalTransform localTransform, LocalToWorld& localToWorld);

        TransformSystem(): System(PostUpdateSystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeSystemInstance(TransformSystem)
}