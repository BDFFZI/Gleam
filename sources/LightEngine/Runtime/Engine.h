#pragma once
#include <functional>
#include <vector>

namespace Light
{
    class Engine
    {
    public:
        static void AddLaunchEvent(const std::function<void()>& launchEvent);
        static void Launch();
    private:
        inline static std::vector<std::function<void()>> launchEvents;
    };
}
