#include "AnimationClip.h"

#include <algorithm>

#include "GleamECS/Runtime/World.h"
#include "GleamMath/Runtime/Math.h"
#include "GleamUtility/Runtime/String.h"

namespace Gleam
{
    int AnimationClip::GetCurveCount() const
    {
        return static_cast<int>(curves.size());
    }
    float AnimationClip::GetAnimationLength()
    {
        if (curves.empty())
            return 0;

        return std::ranges::max(curves | std::views::transform([](AnimationCurve& curve)
        {
            return curve.GetTimeSpan();
        }));
    }
    void AnimationClip::SampleAnimation(const Entity entity, float time)
    {
        //后处理时间
        float animationLength = GetAnimationLength();
        switch (timeWrapMode)
        {
        case TimeWrapMode::Clamp:
            time = std::clamp(time, 0.0f, animationLength);
            break;
        case TimeWrapMode::Loop:
            time = Repeat(time, animationLength);
            break;
        case TimeWrapMode::PingPong:
            time = PingPong(time, animationLength);
            break;
        }

        //缓存运行时字段数据
        if (runtimeData.empty())
            CalculateRuntimeData();

        const EntityInfo& entityInfo = World::GetEntityInfoAllocator().GetEntityInfo(entity);
        for (const auto& [curveIndex, componentType,fieldOffset,fieldType] : runtimeData)
        {
            if (!entityInfo.archetype->HasComponent(componentType))
                continue; //实体原型中没有目标组件，跳过
            //计算字段地址
            void* fieldAddress =
                entityInfo.memoryAddress +
                entityInfo.archetype->GetComponentOffset(componentType) +
                fieldOffset;
            //计算曲线值并应用到实体数据
            float value = curves[curveIndex].Evaluate(time);
            if (fieldType == typeid(float))
                *static_cast<float*>(fieldAddress) = value;
            else if (fieldType == typeid(int))
                *static_cast<int*>(fieldAddress) = static_cast<int>(value);
            else if (fieldType == typeid(bool))
                *static_cast<bool*>(fieldAddress) = static_cast<bool>(value);
        }
    }
    void AnimationClip::CalculateRuntimeData()
    {
        runtimeData.clear();

        int curveCount = GetCurveCount();
        for (int curveIndex = 0; curveIndex < curveCount; curveIndex++)
        {
            //获取组件
            auto optionalComponent = Type::GetType(components.at(curveIndex));
            if (!optionalComponent.has_value())
                continue; //组件未注册反射，无法获取内部字段，跳过
            const Type& component = optionalComponent.value();
            //获取字段
            static std::vector<FieldInfo> fields;
            if (component.FindFields(properties.at(curveIndex), fields) == false)
                continue; //无法获取到所有字段，跳过
            //计算字段偏移
            int fieldOffset = std::accumulate(fields.begin(), fields.end(), 0, [](const int sum, const FieldInfo& field)
            {
                return sum + field.offset;
            });

            runtimeData.emplace_back(curveIndex, component.GetIndex(), fieldOffset, fields.back().type);
        }
    }
}