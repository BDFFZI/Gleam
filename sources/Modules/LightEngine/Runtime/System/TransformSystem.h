#pragma once
#include "UpdateSystem.h"
#include "LightECS/Runtime/System.h"
#include "LightEngine/Runtime/Component/Transform.h"

namespace Light
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
    Light_MakeGlobalSystem(TransformSystem)
}