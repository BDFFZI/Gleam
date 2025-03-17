#include "ParticleEditingSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"

namespace Gleam
{
    void ParticleEditingSystem::Update()
    {
        View<Particle>::Each([](Particle& particle)
        {
            particle.lastPosition = particle.position;
        });
    }
}