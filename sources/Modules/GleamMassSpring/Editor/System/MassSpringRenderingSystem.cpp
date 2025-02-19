#include "MassSpringRenderingSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Component/SpringPhysics.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"
#include "GleamRendering/Editor/Gizmos.h"

namespace Gleam
{
    void MassSpringRenderingSystem::Update()
    {
        View<Point, MassPointPhysics>::Each([this](Point& point, MassPointPhysics& pointPhysics)
        {
            Gizmos::DrawPoint(point, float4{1, 1 - pointPhysics.drag, 1 - pointPhysics.drag, 1});
        });
        View<SpringPhysics>::Each([this](SpringPhysics& springPhysics)
        {
            Point pointA;
            MassPointPhysics massPointPhysicsA;
            springPhysics.pointA.Get(pointA, massPointPhysicsA);
            Point pointB;
            MassPointPhysics massPointPhysicsB;
            springPhysics.pointB.Get(pointB, massPointPhysicsB);

            Gizmos::DrawSegment(Segment{pointA.position, pointB.position});
        });
    }
}