#include "Window.h"

#include <exception>

#include "Input.h"
#include "Time.h"
#include "LightEngine/Runtime/Engine.h"

using namespace Light;

Window Window::Initialize(const char* name, const int width, const int height, const bool fullscreen)
{
    Input::Initialize(glfwWindow);

    return {};
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

void Window::SetWindowStartEvent(const std::function<void()>& windowStartEvent)
{
    if (windowStartEvent == nullptr)
        throw std::exception("事件为空");

    Window::windowStartEvent = windowStartEvent;
}
void Window::SetWindowStopEvent(const std::function<void()>& windowStopEvent)
{
    if (windowStopEvent == nullptr)
        throw std::exception("事件为空");

    Window::windowStopEvent = windowStopEvent;
}
void Window::SetWindowUpdateEvent(const std::function<void()>& windowUpdateEvent)
{
    if (windowUpdateEvent == nullptr)
        throw std::exception("事件为空");

    Window::windowUpdateEvent = windowUpdateEvent;
}
void Window::SetWindowStopConfirm(const std::function<bool()>& windowStopConfirm)
{
    if (windowStopConfirm == nullptr)
        throw std::exception("事件为空");

    Window::windowStopConfirm = windowStopConfirm;
}
void Window::SetFullScreen(const bool fullscreen)
{
    int2 resolution = GetResolution();
    if (fullscreen)
        glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, resolution.x, resolution.y,GLFW_DONT_CARE);
    else
        glfwSetWindowMonitor(glfwWindow, nullptr, 50, 50, resolution.x, resolution.y,GLFW_DONT_CARE);
}
void Window::SetResolution(const int width, const int height)
{
    if (GetFullScreen())
        glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, width, height,GLFW_DONT_CARE);
    else
        glfwSetWindowSize(glfwWindow, width, height);
}

void Window::Start()
{
    if (!glfwInit())
        throw std::exception("窗口初始化失败");

    const char* name = "Window";
    constexpr int width = 1920 / 2;
    constexpr int height = 1080 / 2;
    constexpr bool fullscreen = false;
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); //使用vulkan，故去除glfw自带的接口
    glfwWindow = glfwCreateWindow(width, height, name, fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);
    if (!glfwWindow)
    {
        glfwTerminate();
        throw std::exception("窗口创建失败");
    }

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
    SystemGroup::Update();

    if (glfwWindowShouldClose(glfwWindow))
        Engine::Stop();
}