#pragma once
#include <limits>

#include "../../../Light/samples/MassSpring/Component.hpp"
#include "LightECS/Runtime/View.hpp"

constexpr float SystemMinOrder = std::numeric_limits<float>::min();
constexpr float SystemMaxOrder = std::numeric_limits<float>::max();

template <float MinOrder, float MaxOrder>
class System
{
public:
    static constexpr float Order = (MinOrder + MaxOrder) / 2;
    virtual void Update() = 0;
};

class RenderingSystem : public System<SystemMinOrder, SystemMaxOrder>
{
    void Update() override
    {
        View<Line> view;
        
    }
};

class PhysicsSystem : public System<RenderingSystem::Order, SystemMaxOrder>
{
};
