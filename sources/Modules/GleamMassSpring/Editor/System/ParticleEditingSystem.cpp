#include "ParticleEditingSystem.h"

#include "GleamECS/Runtime/View/View.h"
#include "GleamMassSpring/Runtime/Entity/Particle.h"

namespace Gleam
{
    void ParticleEditingSystem::Update()
    {
        if (Editor::GetIsPlaying())
            return;

        World::GetView<Particle>().Each([](Particle& particle)
        {
            particle.lastPosition = particle.position;
        });
    }
}