#pragma once
#include <string>
#include <vector>

#include "AnimationCurve.h"
#include "GleamReflection/Runtime/Object.h"

namespace Gleam
{
    enum class TimeWrapMode
    {
        Clamp,
        Loop,
        PingPong
    };

    class AnimationClip
    {
    public:
        int GetCurveCount() const;
        float GetAnimationLength();

        void SampleAnimation(Object& object, float time);

    private:
        std::vector<std::string> properties;
        std::vector<AnimationCurve> curves;
        TimeWrapMode timeWrapMode = TimeWrapMode::Clamp;
    };
}