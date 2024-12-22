#pragma once
#include <functional>
#include <GLFW/glfw3.h>

#include "LightECS/Runtime/System.h"
#include "LightMath/Runtime/Vector.hpp"

namespace Light
{
    class Window : public SystemGroup
    {
    public:
        Window(): SystemGroup(nullptr, LeftOrder, MiddleOrder)
        {
        }

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

    private:
        GLFWwindow* glfwWindow;

        void Start() override;
        void Stop() override;
        void Update() override;
    };
}