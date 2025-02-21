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

        //胡克定律：弹力=弹力系数*距离*方向
        //使用质点结算了当前速度和力后的位置信息计算参数，这种基于隐式欧拉的计算方法可以提高模拟稳定性，
        //因为它能考虑到与当前其他力的相互作用，从而能实现一定的弹簧阻力，否则得额外实现弹簧阻力。
        //但这种阻力也有缺点，它会使质点不再遵循能量守恒，在单摆实验中无法回到相同的高度，而其他实现阻力的方式没有这种情况。
        float3 vector = particleA.GetNextPosition() - particleB.GetNextPosition();
        // float3 vector = particleA.position - particleB.position;//或者使用非隐式欧拉再配合下面额外的阻力实现
        float distance = max(length(vector), std::numeric_limits<float>::epsilon());
        float tendency = distance - spring.length; //距离（可为负数来使方向取反）
        float3 direction = vector / distance; //方向
        //由于输入的弹力系数是基于位移的，此处得到的“弹力”实际是位移。
        //当弹力系数为1时，得到的位移后顶点是最佳约束点，因为它完美按照了弹簧的距离要求进行位移。
        //若非完美约束点，那最远也不能超过最佳约束点1倍距离，因为大于1后每次迭代只会越来越远，最终弹簧将崩溃。
        //考虑添加力后将累计速度，速度也会产生位移，除非有足以抵消的阻力（包括隐式欧拉导致的阻力），否则弹力系数不能超过1。
        float3 move = spring.elasticity * tendency * direction;
        //考虑不同阻力下的弹簧约束效果
        float oneMinusDragB = 1 - particleB.drag;
        float oneMinusDragA = 1 - particleA.drag;
        if (equal(oneMinusDragB + oneMinusDragA, 0.0f))
        {
            //基于不受阻力影响的位移结算，这样可以模拟完全刚性的距离约束，但会导致碰撞体的摩擦力约束失败
            move *= 0.5f / particleB.mass; //乘0.5是因为两端质点共同平摊位移效果
            particleB.position += move;
            particleA.position -= move;
            //要不是阻力为都1，还需要修改lastPosition，否则每次迭代会额外步进一次
        }
        else
        {
            //两端质点共同平摊位移效果，根据阻力分配权重，以便在阻力影响下也能正确达到约束位置。
            float moveWeight = oneMinusDragB / (oneMinusDragB + oneMinusDragA);
            //基于力结算
            //这里的弹力系数是基于位移的，故将其转为和力（加速度）同阶的大小。乘0.5则是因为有两个端点平摊位移。
            float3 force = 1.0f / TimeSystem.GetFixedDeltaTime() / TimeSystem.GetFixedDeltaTime() * move;
            particleB.force += moveWeight * force;
            particleA.force -= (1 - moveWeight) * force;

            //基于速度结算
            // particleB.lastPosition -= moveWeight * oneMinusDragB / particleB.mass * move;
            // particleA.lastPosition += (1 - moveWeight) * oneMinusDragA / particleA.mass * move;
        }

        // //不采用隐式欧拉实现弹簧阻力时，需单独实现阻力功能，以便处理弹簧之间的相互作用。
        // float3 forceB = particleB.lastForce + particleB.force;
        // float3 forceA = particleA.lastForce + particleA.force;
        // float forceBA = dot(forceB - forceA, direction); //使B向A接近的力总和
        // float3 resistance = -direction * spring.elasticity * 0.1f * forceBA;
        // particleB.force += resistance;
        // particleA.force -= resistance;
    });

    //重力
    View<Particle>::Each([](Particle& particle)
    {
        particle.force += PhysicsSystem.GetGravity() * particle.mass;
    });
}