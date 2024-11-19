#pragma once

namespace Light
{
    class Window;
    class Time
    {
    public:
        static float GetTimeReal();
        static float GetTime();
        static float GetDeltaTime();
        static int GetFrameCount();

    private:
        inline static float timeScale = 1;
        inline static int frameCount = -1;
        inline static float lastTimeReal;
        inline static float deltaTime;
        inline static float time;

        friend Window;
        static void Start();
        static void Update();
    };
}
