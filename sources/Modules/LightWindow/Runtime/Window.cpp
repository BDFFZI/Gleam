#include "Window.h"

#include <algorithm>
#include <exception>
#include "LightEngine/Runtime/Engine.h"

using namespace Light;

int2 Window::GetResolution() const
{
    int2 resolution;
    glfwGetWindowSize(glfwWindow, &resolution.x, &resolution.y);
    return resolution;
}
bool Window::GetFullScreen() const
{
    return glfwGetWindowMonitor(glfwWindow) != nullptr;
}
void Window::SetResolution(const int2 resolution) const
{
    if (GetFullScreen())
        glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, resolution.x, resolution.y,GLFW_DONT_CARE);
    else
        glfwSetWindowSize(glfwWindow, resolution.x, resolution.y);
}
void Window::SetFullScreen(const bool fullscreen) const
{
    const int2 resolution = GetResolution();
    if (fullscreen)
        glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, resolution.x, resolution.y,GLFW_DONT_CARE);
    else
        glfwSetWindowMonitor(glfwWindow, nullptr, 50, 50, resolution.x, resolution.y,GLFW_DONT_CARE);
}
void Window::SetMousePosition(const float2 position) const
{
    glfwSetCursorPos(glfwWindow, position.x, position.y);
}

void Window::Start()
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

    SystemGroup::Start();
}
void Window::Stop()
{
    SystemGroup::Stop();

    glfwDestroyWindow(glfwWindow);

    glfwTerminate();
}
void Window::Update()
{
    glfwPollEvents();

    bool currentMouseState = std::ranges::any_of(mouseButtonStates, [](auto item) { return item; });
    isMouseDown = lastMouseState == false && currentMouseState == true;
    lastMouseState = currentMouseState;

    SystemGroup::Update();

    if (glfwWindowShouldClose(glfwWindow))
        Engine::Stop();
}

void Light::GlfwWindowFocusCallback(GLFWwindow* window, int focused)
{
}
void Light::GlfwCursorEnterCallback(GLFWwindow* window, int entered)
{
}
void Light::GlfwCursorPosCallback(GLFWwindow* window, const double xPos, const double yPos)
{
    Window->mousePosition = float2{static_cast<float>(xPos), static_cast<float>(yPos)};
}
void Light::GlfwScrollCallback(GLFWwindow* window, const double xOffset, const double yOffset)
{
    Window->mouseScrollDelta = {static_cast<float>(xOffset), static_cast<float>(yOffset)};
}
void Light::GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, int mods)
{
    Window->mouseButtonStates[button] = action != GLFW_RELEASE;
}
void Light::GlfwKeyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
{
    Window->keyboardStates[key] = action != GLFW_RELEASE;
}
void Light::GlfwCharCallback(GLFWwindow* window, unsigned int codepoint)
{
}