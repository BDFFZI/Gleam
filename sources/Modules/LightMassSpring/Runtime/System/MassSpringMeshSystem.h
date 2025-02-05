#pragma once
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
        
        void Update() override;
    };
    Light_MakeSystem(MassSpringMeshSystem)
}