#pragma once
#include <vector>

#include "GleamReflection/Runtime/Type.h"

namespace Gleam
{
    class AnimationCurve
    {
    public:
        int GetKeyCount() const;
        float GetTimeSpan() const;
        void AddKey(float time, float value);
        void RemoveKey(int index);
        float Evaluate(float time) const;

    private:
        std::vector<float> times;
        std::vector<float> values;
    };

    Gleam_MakeType(AnimationCurve, "25C73BFA-3356-4334-82CF-B2E22F24CE15")
    {
    }
}