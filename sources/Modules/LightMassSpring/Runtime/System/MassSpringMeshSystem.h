#pragma once

//质点弹簧可视化，当额外引用LightRenderingRuntime库时激活该功能，便于调试
#ifdef LightRenderingRuntime
#include "LightECS/Runtime/View.h"
#include "LightMassSpring/Runtime/Component/MassPointPhysics.h"
#include "LightMassSpring/Runtime/Component/MassSpringMesh.h"
#include "LightMassSpring/Runtime/Component/SpringPhysics.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"
#include "PhysicsSystem.h"
#include "LightMath/Runtime/Geometry/Point.h"
#include "LightMath/Runtime/Geometry/Segment.h"

namespace Light
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
                springPhysics.pointA.Get(&pointA, &massPointPhysicsA);
                Point pointB;
                MassPointPhysics massPointPhysicsB;
                springPhysics.pointB.Get(&pointB, &massPointPhysicsB);

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
    Light_MakeSystemInstance(MassSpringMeshSystem)
}
#endif