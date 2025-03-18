#pragma once
#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class ParticleEditingSystem : public System
    {
    public:
        ParticleEditingSystem(): System(GlobalPostUpdateSystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(ParticleEditingSystem)
}