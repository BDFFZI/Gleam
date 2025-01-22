#pragma once
#include <chrono>

namespace Light
{
    template <class TUnit = std::chrono::milliseconds>
    class Timer
    {
    public:
        Timer()
        {
            Tick();
        }

        int Tick()
        {
            auto durationTime = Time();
            lastTime = std::chrono::high_resolution_clock::now();
            return durationTime;
        }
        int Time()
        {
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto duration = currentTime - lastTime;
            auto durationTime = std::chrono::duration_cast<TUnit>(duration).count();
            return static_cast<int>(durationTime);
        }

    private:
        std::chrono::high_resolution_clock::time_point lastTime;
    };
}
