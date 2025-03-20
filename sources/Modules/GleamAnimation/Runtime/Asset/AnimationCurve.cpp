#include "AnimationCurve.h"

#include <cassert>
#include <cmath>

namespace Gleam
{
    int AnimationCurve::GetKeyCount() const
    {
        return static_cast<int>(times.size());
    }
    float AnimationCurve::GetTimeSpan() const
    {
        return std::ranges::max(times);
    }
    void AnimationCurve::AddKey(float time, const float value)
    {
        auto it = std::ranges::find_if(times, [time](const float frameTime)
        {
            return frameTime > time;
        });
        times.insert(it, value);
    }
    void AnimationCurve::RemoveKey(const int index)
    {
        assert(index >= 0 && index < GetKeyCount() && "索引超出范围！");

        times.erase(times.begin() + index);
        values.erase(values.begin() + index);
    }
    float AnimationCurve::Evaluate(float time) const
    {
        if (times.empty())
            return 0;

        auto it = std::ranges::find_if(times, [time](const float frameTime)
        {
            return frameTime > time;
        });
        if (it == times.end())
            return values.back();
        if (it == times.begin())
            return values.front();

        auto index = it - times.begin();
        auto rate = (time - times[index - 1]) / (times[index] - times[index - 1]);
        return std::lerp(values[index - 1], values[index], rate);
    }
}