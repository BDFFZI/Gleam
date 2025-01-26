#pragma once

namespace Light
{
    class Engine
    {
    public:
        static void Start();
        static void Stop();

    private:
        inline static bool isStopping = false;
    };
}