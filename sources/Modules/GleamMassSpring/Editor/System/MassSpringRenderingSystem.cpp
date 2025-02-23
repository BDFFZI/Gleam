#include "MassSpringRenderingSystem.h"
#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Component/Spring.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"
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

        View<Particle>::Each([](Particle& particle)
        {
            Gizmos::Draw(particle.lastPosition, float4::Blue());
            Gizmos::Draw(particle.position, float4{1, 1 - particle.drag, 1 - particle.drag, 1});
        });
        View<Spring>::Each([](Spring& springPhysics)
        {
            Particle& particleA = World::GetComponent<Particle>(springPhysics.particleA);
            Particle& particleB = World::GetComponent<Particle>(springPhysics.particleB);
            Gizmos::Draw(Segment{particleA.position, particleB.position});
        });
        View<Rectangle, Collider>::Each([](Entity& entity, Rectangle& rectangle, Collider&)
        {
            DrawRectangle(entity, rectangle, float4::DarkGreen());
        });
        View<Cuboid, Collider>::Each([](Entity& entity, Cuboid& cuboid, Collider&)
        {
            DrawCuboid(entity, cuboid, float4::DarkGreen());
        });
        View<Sphere, Collider>::Each([](Entity& entity, Sphere& sphere, Collider&)
        {
            DrawSphere(entity, sphere, float4::DarkGreen());
        });
    }
}