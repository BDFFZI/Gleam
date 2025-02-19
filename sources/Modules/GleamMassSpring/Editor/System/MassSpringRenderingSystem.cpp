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
            Point pointA = World::GetComponent<Point>(springPhysics.pointA);
            Point pointB = World::GetComponent<Point>(springPhysics.pointB);
            Gizmos::DrawSegment(Segment{pointA.position, pointB.position});
        });
    }
}