#pragma once
#include <string>
#include <vector>

#include "AnimationCurve.h"
#include "GleamECS/Runtime/Archetype.h"

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

        void SampleAnimation(Entity entity, float time);
        void CalculateRuntimeData();

    private:
        Gleam_MakeType_Friend
        std::vector<uuids::uuid> components;
        std::vector<std::string> properties;
        std::vector<AnimationCurve> curves;
        TimeWrapMode timeWrapMode = TimeWrapMode::Clamp;

        std::vector<std::tuple<int, std::type_index, int, std::type_index>> runtimeData;
    };

    Gleam_MakeType(AnimationClip, "DBBDA60F-07AA-4C3A-A9F0-7D37DAD679FA")
    {
        Gleam_MakeType_AddField(components);
        Gleam_MakeType_AddField(properties);
        Gleam_MakeType_AddField(curves);
        Gleam_MakeType_AddField(timeWrapMode);
    }
}