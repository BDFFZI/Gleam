#include "Profiler.h"

namespace Gleam
{
    Profile& Profile::NextChild()
    {
        Profile* child;
        if (static_cast<int>(children.size()) > childrenCount + 1)
            child = &children[childrenCount];
        else
            child = &children.emplace_back();

        childrenCount++;
        return *child;
    }
    void Profile::Initialize(const std::string_view name)
    {
        this->name = name;
        time = 0.0f;
        childrenCount = 0;
        timer.Tick();
    }

    ProfileSampler::ProfileSampler(const std::string_view name)
    {
        Profiler::Begin(name);
    }
    ProfileSampler::~ProfileSampler()
    {
        Profiler::End();
    }

    Profile Profiler::FlushProfile()
    {
        Profile profile = rootProfile;
        rootProfile.Initialize("");
        return profile;
    }

    ProfileSampler Profiler::Sample(const std::string_view name)
    {
        return ProfileSampler(name);
    }
    void Profiler::Begin(const std::string_view name)
    {
        Profile& profile = profilePath.back()->NextChild();
        profile.Initialize(name);
        profilePath.emplace_back(&profile);
    }
    void Profiler::End()
    {
        Profile& profile = *profilePath.back();
        profile.time = static_cast<float>(profile.timer.Time()) / 1000.0f;
        profilePath.pop_back();
    }
}