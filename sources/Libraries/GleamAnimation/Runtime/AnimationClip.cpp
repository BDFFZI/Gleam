#include "AnimationClip.h"

namespace Gleam
{
    int AnimationClip::GetCurveCount() const
    {
        return curves.size();
    }
    float AnimationClip::GetAnimationLength()
    {
        return std::ranges::max(curves | std::views::transform([](AnimationCurve& curve)
        {
            return curve.GetTimeSpan();
        }));
    }
    void AnimationClip::SampleAnimation(Object& object, float time)
    {
        switch (timeWrapMode)
        {
        case TimeWrapMode::Clamp:
            break;
        case TimeWrapMode::Loop:
            break;
        case TimeWrapMode::PingPong:
            break;
        }
    }
}