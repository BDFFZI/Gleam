#include "MassSpringMeshSystem.h"

#include <iostream>

#include "LightECS/Runtime/View.h"
#include "LightMassSpring/Runtime/Component/MassPointPhysics.h"
#include "LightMassSpring/Runtime/Component/MassSpringMesh.h"
#include "LightMassSpring/Runtime/Component/SpringPhysics.h"
#include "LightRendering/Runtime/Component/LinesMesh.h"
#include "LightRendering/Runtime/Component/PointsMesh.h"

namespace Light
{
    void MassSpringMeshSystem::Update()
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
}