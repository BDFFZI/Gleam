#pragma once
#include <functional>
#include <GLFW/glfw3.h>

namespace LightEngine
{
    class Window
    {
    public:
        /**
        * 初始化并创建窗口
        */
        static void Initialize();

        static void SetWindowCloseEvent(const std::function<bool()>& windowCloseEvent);
        static void SetWindowBeginEvent(const std::function<void()>& windowBeginEvent);
        static void SetWindowEndEvent(const std::function<void()>& windowEndEvent);
        static void SetWindowUpdateEvent(const std::function<void()>& windowUpdateEvent);
        
        /**
         * 执行窗口更新逻辑
         */
        static void Begin();
        /**
         * 尝试关闭窗口
         */
        static void End();
    private:
        inline static GLFWwindow* glfwWindow;
        inline static std::function<bool()> windowCloseEvent = [] { return true; };
        inline static std::function<void()> windowBeginEvent = []
        {
        };
        inline static std::function<void()> windowEndEvent = []
        {
        };
        inline static std::function<void()> windowUpdateEvent = []
        {
        };
    };
}
