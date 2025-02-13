#pragma once

//质点弹簧可视化，当额外引用GleamRenderingRuntime库时激活该功能，便于调试
#ifdef GleamRenderingRuntime
#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/MassPointPhysics.h"
#include "GleamMassSpring/Runtime/Component/MassSpringMesh.h"
#include "GleamMassSpring/Runtime/Component/SpringPhysics.h"
#include "GleamRendering/Runtime/Component/LinesMesh.h"
#include "GleamRendering/Runtime/Component/PointsMesh.h"
#include "PhysicsSystem.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"
#include "GleamMath/Runtime/Geometry/Solid/Segment.h"

namespace Gleam
{
    class MassSpringMeshSystem : public System
    {
    public:
        MassSpringMeshSystem(): System(PhysicsSystem, OrderRelation::After)
        {
        }

    private:
        std::vector<Point> massPoints;
        std::vector<Segment> springs;

        void Update() override
        {
            massPoints.clear();
            View<Point, MassPointPhysics>::Each([this](Point& point, MassPointPhysics& pointPhysics)
            {
                massPoints.push_back(point);
            });
            springs.clear();
            View<SpringPhysics>::Each([this](SpringPhysics& springPhysics)
            {
                Point pointA;
                MassPointPhysics massPointPhysicsA;
                springPhysics.pointA.Get(pointA, massPointPhysicsA);
                Point pointB;
                MassPointPhysics massPointPhysicsB;
                springPhysics.pointB.Get(pointB, massPointPhysicsB);

                springs.emplace_back(pointA.position, pointB.position);
            });


            View<MassSpringMesh, PointsMesh>::Each([this](MassSpringMesh&, PointsMesh& pointsMesh)
            {
                pointsMesh.points = massPoints;
            });
            View<MassSpringMesh, LinesMesh>::Each([this](MassSpringMesh&, LinesMesh& linesMesh)
            {
                linesMesh.lines = springs;
            });
        }
    };
    Gleam_MakeGlobalSystem(MassSpringMeshSystem)
}
#endif