#pragma once
#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class ParticleEditingSystem : public System<PostUpdateSystem>
    {
        void Update() override;
    };
    Gleam_MakeEditorSystem(ParticleEditingSystem)
}