#include "ForceSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMassSpring/Runtime/Component/Spring.h"

void Gleam::ForceSystem::Update()
{
    //弹簧力
    View<Spring>::Each([](Spring& spring)
    {
        Particle& particleA = World::GetComponent<Particle>(spring.particleA);
        Particle& particleB = World::GetComponent<Particle>(spring.particleB);

        //空气阻力不能为0是因为后面会用他做除法，来判断两端质点的位移权重
        float dragA = max(std::numeric_limits<float>::epsilon(), 1 - particleA.drag);
        float dragB = max(std::numeric_limits<float>::epsilon(), 1 - particleB.drag);
        //使用质点结算速度后的位置信息，这种基于隐式欧拉的计算方法可以提高模拟稳定性，因为它能考虑到与当前其他力的相互作用，从而能模拟一定的弹簧阻力
        float3 positionA = particleA.position + (particleA.position - particleA.lastPosition) * dragA;
        float3 positionB = particleA.position + (particleA.position - particleA.lastPosition) * dragB;
        //计算最佳约束点，最佳约束点即完全满足弹簧长度要求的位置，质点到此位置时将不会受到任何弹力
        float3 vector = positionA - positionB;
        float tendency = length(vector) - spring.length;
        float3 direction = normalize(vector) * sign(tendency); //最佳约束位置所在方向
        float distance = std::abs(tendency); //离最佳约束位置的距离
        //计算因弹簧弹力性质影响的实际约束点。正因为不会一次性抵达最佳约束点，所以在会有弹来弹去的效果。
        //实际约束点最远不能超过最佳约束点1倍距离，因为大于1后每次迭代只会越来越远，最终弹簧将崩溃。
        float move = std::lerp(0.0f, 2 * distance, spring.elasticity * 0.5f);
        //弹簧会使两端质点一起位移来满足约束要求。要考虑两端质点阻力不同，分配不同的位移权重，
        //以便在阻力影响下也能正确达到约束位置，同时这也可以防止固定点被拉拽。
        float moveWeight = std::lerp(0.0f, 1.0f, dragB / (dragB + dragA));
        particleB.position += move * moveWeight / particleB.mass * direction;
        particleA.position += move * (1 - moveWeight) / particleA.mass * -direction;
    });

    //重力
    View<Particle>::Each([](Particle& particle)
    {
        particle.position += PhysicsSystem.GetGravity() * TimeSystem.GetFixedDeltaTime() * TimeSystem.GetFixedDeltaTime();
    });
}