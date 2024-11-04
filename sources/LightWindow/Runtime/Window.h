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

        static void SetWindowStartEvent(const std::function<void()>& windowBeginEvent);
        static void SetWindowUpdateEvent(const std::function<void()>& windowUpdateEvent);
        static void SetWindowStopEvent(const std::function<void()>& windowEndEvent);
        static void SetWindowStopConfirm(const std::function<bool()>& windowStopConfirm);

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
