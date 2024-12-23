#include "Window.h"
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
void Window::SetFullScreen(const bool fullscreen) const
{
    const int2 resolution = GetResolution();
    if (fullscreen)
        glfwSetWindowMonitor(glfwWindow, glfwGetPrimaryMonitor(), 0, 0, resolution.x, resolution.y,GLFW_DONT_CARE);
    else
        glfwSetWindowMonitor(glfwWindow, nullptr, 50, 50, resolution.x, resolution.y,GLFW_DONT_CARE);
}
void Window::SetResolution(const int width, const int height) const
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

    //初始化配置
    // ReSharper disable CppLocalVariableMayBeConst
    const char* name = "Window";
    int width = 1920 / 2;
    int height = 1080 / 2;
    bool fullscreen = false;
    // ReSharper restore CppLocalVariableMayBeConst

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