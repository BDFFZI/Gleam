#pragma once
#include <functional>
#include <GLFW/glfw3.h>

#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    class Window
    {
    public:
        /**
         * @brief 初始化并创建窗口
         * @param name
         * @param width
         * @param height
         * @param fullscreen
         */
        static Window Initialize(const char* name, int width, int height, bool fullscreen);

        static int2 GetResolution();
        static bool GetFullScreen();
        static GLFWwindow* GetGlfwWindow() { return glfwWindow; }
        static void SetWindowStartEvent(const std::function<void()>& windowStartEvent);
        static void SetWindowStopEvent(const std::function<void()>& windowStopEvent);
        static void SetWindowUpdateEvent(const std::function<void()>& windowUpdateEvent);
        static void SetWindowStopConfirm(const std::function<bool()>& windowStopConfirm);
        static void SetResolution(int width, int height);
        static void SetFullScreen(bool fullscreen);

        /**
         * 执行窗口更新逻辑
         */
        static void Start();
        /**
         * 尝试关闭窗口
         */
        static void Stop();

    private:
        inline static GLFWwindow* glfwWindow;
        inline static std::function<void()> windowStartEvent = []
        {
        };
        inline static std::function<void()> windowUpdateEvent = []
        {
        };
        inline static std::function<void()> windowStopEvent = []
        {
        };
        inline static std::function<bool()> windowStopConfirm = [] { return true; };

        Window() = default;
    };
}
