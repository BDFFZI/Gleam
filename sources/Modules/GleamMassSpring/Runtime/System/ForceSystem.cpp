#include "ForceSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMassSpring/Runtime/Component/Spring.h"

void Gleam::ForceSystem::Update()
{
    //重力
    View<Particle>::Each([this](Particle& particle)
    {
        float3 move = GetGravity() * TimeSystem.GetFixedDeltaTime() * TimeSystem.GetFixedDeltaTime()
            * (1 - particle.drag);
        particle.position += move;
    });

    //弹簧力
    View<Spring>::Each([](Spring& spring)
    {
        Particle& particleA = World::GetComponent<Particle>(spring.particleA);
        Particle& particleB = World::GetComponent<Particle>(spring.particleB);

        //使用质点结算了当前速度和力后的位置信息来计算参数（这里力始终直接施加在位移上），这种类似半隐式欧拉的计算方法可以提高模拟稳定性，
        //因为它能考虑到与当前其他力的相互作用，从而能实现一定的弹簧阻力，否则得额外实现弹簧阻力。
        //但这种阻力也有缺点，它会使质点不再遵循能量守恒（释放的弹力会因其他力变化，而不是基于距离的定值），在单摆实验中无法回到相同的高度，而其他实现阻力的方式没有这种问题。
        float3 vector = particleA.position - particleB.position;
        // 下面这条注释使用的质点位置是当前帧尚未结算过速度或其他力的位置，也就是说不会受其他力影响，只基于瞬时距离计算，虽然结果不稳定，需要额外配备阻力功能，但它遵循能量守恒定律
        // float3 vector = particleA.lastPosition - particleB.lastPosition;
        if (all(vector == 0))vector = {0, std::numeric_limits<float>::epsilon(), 0};
        float distance = length(vector);
        //胡克定律：弹力=弹力系数*距离*方向
        float tendency = distance - spring.length; //距离（可为负数来使方向取反）
        float3 direction = vector / distance; //方向
        //弹力系数实际是有上下限的，若当前质点基于约束位置距离为a，则最高不能超过2a，因为超过后每次迭代，离约束点的位置只会越来越远。
        //故此处的弹力系数是基于位移的，范围为[0,2]，代表的就是上述的范围限制，因此此处得到的“弹力”实际是位移。
        //当弹力系数为1时，得到的位移后顶点是最佳约束点，因为它完美按照了弹簧的距离要求进行位移。
        //若非完美约束点，那最远也不能超过最佳约束点1倍距离，因为大于1后每次迭代只会越来越远，最终弹簧将崩溃。
        //考虑添加力后将累计速度，速度也会产生位移，除非有足以抵消的阻力（包括半隐式欧拉积分法导致的阻力），否则弹力系数不能超过1。
        float3 move = spring.elasticity * tendency * direction;
        //弹簧会使两端质点一起位移来满足约束要求。要考虑两端质点阻力不同，分配不同的位移权重，
        //以便在阻力影响下也能正确达到约束位置，同时这也可以防止固定点被拉拽。
        float oneMinusDragB = 1 - particleB.drag;
        float oneMinusDragA = 1 - particleA.drag;
        float moveWeight = equal(oneMinusDragB + oneMinusDragA, 0.0f) ? 0.5f : oneMinusDragB / (oneMinusDragB + oneMinusDragA);
        particleB.position += moveWeight / particleB.mass * move; //力转换成位移时还会受质量影响，故在此要除上质量系数
        particleA.position += (1 - moveWeight) / particleA.mass * -move;
    });
}