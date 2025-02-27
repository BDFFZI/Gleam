#pragma once
#include <string>
#include "GleamUtility/Runtime/Timer.h"

namespace Gleam
{
    struct Profile
    {
        std::string name;
        float time = 0;
        std::vector<Profile> children = {};
        int childrenCount = 0;
        Timer<std::chrono::microseconds> timer = {};

        Profile& NextChild();
        void Initialize(std::string_view name);
    };

    class ProfileSampler
    {
    public:
        explicit ProfileSampler(std::string_view name);
        ~ProfileSampler();
    };

    class Profiler
    {
    public:
        static Profile FlushProfile();

        static ProfileSampler Sample(std::string_view name);
        static void Begin(std::string_view name);
        static void End();

    private:
        inline static Profile rootProfile = {};
        inline static std::vector<Profile*> profilePath = {&rootProfile};
    };

#define Gleam_ProfilerSample_Inner2(name,id) auto sampler##id = Profiler::Sample(name)
#define Gleam_ProfilerSample_Inner(name,id) Gleam_ProfilerSample_Inner2(name,id)
#define Gleam_ProfilerSample(name) Gleam_ProfilerSample_Inner(name,__LINE__)
}