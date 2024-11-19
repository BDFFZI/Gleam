#include "Window.h"

#include <exception>
#include <iostream>

#include "Input.h"
#include "Time.h"

using namespace Light;

Window Window::Initialize(const char* name, const int width, const int height, const int clientAPI)
{
    if (!glfwInit())
        throw std::exception("窗口初始化失败");

    glfwWindowHint(GLFW_CLIENT_API, clientAPI);
    glfwWindow = glfwCreateWindow(width, height, name, nullptr, nullptr);
    if (!glfwWindow)
    {
        glfwTerminate();
        throw std::exception("窗口创建失败");
    }

    return {};
}

void Window::SetWindowStopConfirm(const std::function<bool()>& windowStopConfirm)
{
    if (windowStopConfirm == nullptr)
        throw std::exception("事件为空");

    Window::windowStopConfirm = windowStopConfirm;
}
void Window::SetWindowStartEvent(const std::function<void()>& windowBeginEvent)
{
    if (windowBeginEvent == nullptr)
        throw std::exception("事件为空");

    windowStartEvent = windowBeginEvent;
}
void Window::SetWindowStopEvent(const std::function<void()>& windowEndEvent)
{
    if (windowEndEvent == nullptr)
        throw std::exception("事件为空");

    windowStopEvent = windowEndEvent;
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
    Input::Start(glfwWindow);
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
