#include "PositionSystem.h"

#include "ForceSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

void Gleam::PositionSystem::Update()
{
    View<Particle>::Each([](Particle& particle)
    {
        float3 currentPosition = particle.position;
        //Verlet积分法（但消去了对加速度的计算，完全基于位移控制质点）
        //由于直接基于位移修改和计算，后续的每次力计算都会自动考虑力的相互作用，因此弹簧将自带阻力。
        //由于直接修改位置实现速度更改，质点积分便不能放在末尾更新，否则会步进两次。
        particle.position = particle.position + (
            particle.position - particle.lastPosition
        ) * (1 - particle.drag); //始终存在误差问题，所以要用阻力修正
        particle.lastPosition = currentPosition;
    });
}