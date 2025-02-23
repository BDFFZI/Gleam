#include "Window.h"

#include <iostream>

namespace Gleam
{
    GLFWwindow* Window::GetGlfwWindow()
    {
        return glfwWindow;
    }
    int2 Window::GetWindowPosition()
    {
        int2 position;
        glfwGetWindowPos(glfwWindow, &position.x, &position.y);
        return position;
    }
    int2 Window::GetResolution()
    {
        int2 resolution;
        glfwGetWindowSize(glfwWindow, &resolution.x, &resolution.y);
        return resolution;
    }
    bool Window::GetFullScreen()
    {
        return glfwGetWindowMonitor(glfwWindow) != nullptr;
    }
    float2 Window::GetMousePosition()
    {
        return mousePosition;
    }
    float2 Window::GetMouseScrollDelta()
    {
        return mouseScrollDelta;
    }
    bool Window::GetIsMouseDown()
    {
        return isMouseDown;
    }
    bool Window::GetMouseButtonState(const MouseButton button)
    {
        return mouseButtonStates[static_cast<uint8_t>(button)];
    }
    bool Window::GetKeyboardState(const KeyCode key)
    {
        return keyboardStates[static_cast<uint16_t>(key)];
    }
    void Window::SetResolution(const int2 resolution)
    {
        if (GetFullScreen())
            glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, resolution.x, resolution.y,GLFW_DONT_CARE);
        else
            glfwSetWindowSize(glfwWindow, resolution.x, resolution.y);
    }
    void Window::SetFullScreen(const bool fullscreen)
    {
        const int2 resolution = GetResolution();
        if (fullscreen)
            glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, resolution.x, resolution.y,GLFW_DONT_CARE);
        else
            glfwSetWindowMonitor(glfwWindow, nullptr, 50, 50, resolution.x, resolution.y,GLFW_DONT_CARE);
    }
    void Window::SetMousePosition(const float2 position)
    {
        glfwSetCursorPos(glfwWindow, position.x, position.y);
    }

    void Window::Init()
    {
        if (glfwInit() == false)
            throw std::exception("窗口初始化失败");
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //使用vulkan，故去除glfw自带的接口

        //默认窗口配置
        // ReSharper disable CppLocalVariableMayBeConst
        const char* name = "Window";
        int width = 1920 / 2;
        int height = 1080 / 2;
        bool fullscreen = false;
        // ReSharper restore CppLocalVariableMayBeConst

        glfwWindow = glfwCreateWindow(width, height, name, fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
        if (!glfwWindow)
            throw std::exception("窗口创建失败");

        glfwSetWindowFocusCallback(glfwWindow, GlfwWindowFocusCallback);
        glfwSetCursorEnterCallback(glfwWindow, GlfwCursorEnterCallback);
        glfwSetCursorPosCallback(glfwWindow, GlfwCursorPosCallback);
        glfwSetMouseButtonCallback(glfwWindow, GlfwMouseButtonCallback);
        glfwSetScrollCallback(glfwWindow, GlfwScrollCallback);
        glfwSetKeyCallback(glfwWindow, GlfwKeyCallback);
        glfwSetCharCallback(glfwWindow, GlfwCharCallback);
    }
    void Window::UnInit()
    {
        glfwDestroyWindow(glfwWindow);
        glfwTerminate();
    }
    void Window::Update()
    {
        mouseScrollDelta = 0;
        glfwPollEvents();

        bool currentMouseState = std::ranges::any_of(mouseButtonStates, [](auto item) { return item; });
        isMouseDown = lastMouseState == false && currentMouseState == true;
        lastMouseState = currentMouseState;
    }

    void Window::GlfwWindowFocusCallback(GLFWwindow* window, int focused)
    {
    }
    void Window::GlfwCursorEnterCallback(GLFWwindow* window, int entered)
    {
    }
    void Window::GlfwCursorPosCallback(GLFWwindow* window, const double xPos, const double yPos)
    {
        mousePosition = float2{static_cast<float>(xPos), static_cast<float>(yPos)};
    }
    void Window::GlfwScrollCallback(GLFWwindow* window, const double xOffset, const double yOffset)
    {
        mouseScrollDelta = {static_cast<float>(xOffset), static_cast<float>(yOffset)};
    }
    void Window::GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, int mods)
    {
        mouseButtonStates[button] = action != GLFW_RELEASE;
    }
    void Window::GlfwKeyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
    {
        keyboardStates[key] = action != GLFW_RELEASE;
    }
    void Window::GlfwCharCallback(GLFWwindow* window, unsigned int codepoint)
    {
    }
}