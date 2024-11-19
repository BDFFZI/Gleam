#pragma once

namespace Light
{
    class Window;
    class Time
    {
    public:
        static float GetTimeReal();
        static float GetTime() { return time; }
        static float GetDeltaTime() { return deltaTime; }
        static int GetFrameCount() { return frameCount; }

        static void SetTimeScale(const float scale) { timeScale = scale; }

    private:
        inline static float timeScale = 1;
        inline static int frameCount = -1;
        inline static float lastTimeReal;
        inline static float deltaTime;
        inline static float time;

        friend Window;
        /**
         * 去除各种初始化时间的干扰，确保从窗口正式更新前开始计时
         */
        static void Start();
        static void Update();
    };
}
