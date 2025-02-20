#include "MassSpringRenderingSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamMassSpring/Runtime/Component/Collider.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Component/SpringPhysics.h"
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

        View<Point, MassPointPhysics, MassPointLastState>::Each([](Entity& entity, Point& point, MassPointPhysics& pointPhysics, MassPointLastState& massPointLastState)
        {
            Gizmos::DrawPoint(point, float4{1, 1 - pointPhysics.drag, 1 - pointPhysics.drag, 1});
            if (entity == InspectorWindow.GetTargetEntity())
                Gizmos::DrawPoint(massPointLastState.lastPosition, float4::Blue());
        });
        View<SpringPhysics>::Each([](SpringPhysics& springPhysics)
        {
            Point pointA = World::GetComponent<Point>(springPhysics.pointA);
            Point pointB = World::GetComponent<Point>(springPhysics.pointB);
            Gizmos::DrawSegment(Segment{pointA.position, pointB.position});
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