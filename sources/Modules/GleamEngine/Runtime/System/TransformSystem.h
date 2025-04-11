#pragma once
#include "UpdateSystem.h"
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

#ifdef GleamEngineEditor
    Gleam_MakeEditorSystem(TransformSystem)
#else
    Gleam_MakeRuntimeSystem(TransformSystem)
#endif
}