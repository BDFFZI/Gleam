#pragma once

namespace Light
{
    class Engine
    {
    public:
        inline static bool isStopping = false;
        inline static bool isUpdating = true;

        static void Start();
        static void Stop();
    };
}