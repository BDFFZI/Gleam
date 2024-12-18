#pragma once
#include <functional>
#include <vector>

namespace Light
{
    class Engine
    {
    public:
        static void AddBeginEvent(const std::function<void()>& beginEvent);
        static void AddEndEvent(const std::function<void()>& endEvent);
        static void AddUpdateEvent(const std::function<void()>& updateEvent);

        static void Initialize();
        static void Begin();
        static void End();

    private:
        inline static std::vector<std::function<void()>> engineBeginEvents;
        inline static std::vector<std::function<void()>> engineEndEvents;
        inline static std::vector<std::function<void()>> engineUpdateEvents;
    };
}
