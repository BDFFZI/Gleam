#include "Window.h"

#include <exception>

#include "Input.h"
#include "Time.h"

using namespace Light;

Window Window::Initialize(const char* name, const int width, const int height)
{
    if (!glfwInit())
        throw std::exception("窗口初始化失败");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!glfwWindow)
    {
        glfwTerminate();
        throw std::exception("窗口创建失败");
    }

    Input::Initialize(glfwWindow);

    return {};
}

void Window::SetWindowStopConfirm(const std::function<bool()>& windowStopConfirm)
{
    if (windowStopConfirm == nullptr)
        throw std::exception("事件为空");

    Window::windowStopConfirm = windowStopConfirm;
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
void Window::SetResolution(const int width, const int height)
{
    glfwSetWindowSize(glfwWindow, width, height);
}

void Window::Start()
{
    Time::Start();

    //启动窗口
    windowStartEvent();

    do
    {
        while (!glfwWindowShouldClose(glfwWindow))
        {
            Time::Update();
            glfwPollEvents();
            Input::Update();

            //更新窗口
            windowUpdateEvent();
        }

        //确定关闭
        glfwSetWindowShouldClose(glfwWindow, windowStopConfirm() ? GLFW_TRUE : GLFW_FALSE);
    }
    while (!glfwWindowShouldClose(glfwWindow));

    //关闭窗口
    windowStopEvent();

    glfwDestroyWindow(glfwWindow);

    glfwTerminate();
}
void Window::Stop()
{
    glfwSetWindowShouldClose(glfwWindow,GLFW_TRUE);
}
