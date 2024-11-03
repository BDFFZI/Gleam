#pragma once
#include <functional>
#include <GLFW/glfw3.h>

namespace Light
{
    class Window
    {
    public:
        /**
         * @brief 初始化并创建窗口
         * @param name
         * @param clientAPI 默认关闭内置的opengl功能，由用户自行控制图形API（以便接入GL模块的vulkan）
         */
        static Window Initialize(const char* name = "Window", int clientAPI = GLFW_NO_API);
        static GLFWwindow* GetGlfwWindow() { return glfwWindow; }

        static void SetWindowCloseEvent(const std::function<bool()>& windowCloseEvent);
        static void SetWindowBeginEvent(const std::function<void()>& windowBeginEvent);
        static void SetWindowEndEvent(const std::function<void()>& windowEndEvent);
        static void SetWindowUpdateEvent(const std::function<void()>& windowUpdateEvent);

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

        Window() = default;
    };
}
