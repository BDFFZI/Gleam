#include "MassSpringRenderingSystem.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamMassSpring/Runtime/Entity/Collider.h"
#include "GleamMassSpring/Runtime/Entity/Particle.h"
#include "GleamMassSpring/Runtime/Entity/Spring.h"
#include "GleamRendering/Editor/CustomUI.h"
#include "GleamRendering/Editor/Gizmos.h"

namespace Gleam
{
    void MassSpringRenderingSystem::SetIsEnabled(const bool state)
    {
        isEnabled = state;
    }

    void MassSpringRenderingSystem::Update()
    {
        if (isEnabled == false)
            return;

        World::GetView<Particle>().Each([](Particle& particle)
        {
            Gizmos::Draw(particle.lastPosition, float4::Blue());
            Gizmos::Draw(particle.position, float4{1, 1 - particle.drag, 1 - particle.drag, 1});
        });
        World::GetView<Spring>().Each([](Spring& springPhysics)
        {
            if (springPhysics.particleA == Entity::Null || springPhysics.particleB == Entity::Null)
                return;
            auto optionalParticleA = World::GetEntityAllocator().TryGetComponent<Particle>(springPhysics.particleA);
            auto optionalParticleB = World::GetEntityAllocator().TryGetComponent<Particle>(springPhysics.particleB);
            if (optionalParticleA.has_value() && optionalParticleB.has_value())
                Gizmos::Draw(Segment{optionalParticleA->get().position, optionalParticleB->get().position});
        });
        World::GetView<Rectangle, Collider>().Each([](Entity& entity, Rectangle& rectangle, Collider&)
        {
            DrawRectangle(entity, rectangle, float4::DarkGreen());
        });
        World::GetView<Cuboid, Collider>().Each([](Entity& entity, Cuboid& cuboid, Collider&)
        {
            DrawCuboid(entity, cuboid, float4::DarkGreen());
        });
        World::GetView<Sphere, Collider>().Each([](Entity& entity, Sphere& sphere, Collider&)
        {
            DrawSphere(entity, sphere, float4::DarkGreen());
        });
    }
}