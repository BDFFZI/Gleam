#include "PositionSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

void Gleam::PositionSystem::Update()
{
    View<Particle>::Each([](Particle& particle)
    {
        //Verlet积分法（但消去了对加速度的计算）
        float3 lastPosition = particle.lastPosition;
        particle.lastPosition = particle.position;
        particle.position = particle.position + (
            particle.position - lastPosition //原本持有的速度
        ) * (1 - particle.drag); //始终存在误差问题，所以要用阻力修正
    });
}