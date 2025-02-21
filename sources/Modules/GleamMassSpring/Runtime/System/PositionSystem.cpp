#include "PositionSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamECS/Runtime/View.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"

void Gleam::PositionSystem::Update()
{
    View<Particle>::Each([](Particle& particle)
    {
        //牛顿第二定律：加速度=力/质量
        float3 acceleration = particle.force / particle.mass;
        particle.force = 0;
        //Verlet积分法（但消去了对加速度的计算）
        float3 lastPosition = particle.lastPosition;
        particle.lastPosition = particle.position;
        particle.position = particle.position + (
            particle.position - lastPosition + //当前速度
            acceleration * TimeSystem.GetFixedDeltaTime() * TimeSystem.GetFixedDeltaTime() //加速度
        ) * (1 - particle.drag); //始终存在误差问题，所以要用阻力修正

        float3 velocity = (particle.position - lastPosition) / (2 * TimeSystem.GetFixedDeltaTime());
        acceleration = velocity / TimeSystem.GetFixedDeltaTime() + acceleration;
        particle.lastForce = acceleration * particle.mass * (1 - particle.drag);
    });
}