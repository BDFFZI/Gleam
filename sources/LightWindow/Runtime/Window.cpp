#include "Window.h"

#include <exception>

#include "Input.h"
#include "Time.h"

using namespace LightEngine;

void Window::SetWindowCloseEvent(const std::function<bool()>& windowCloseEvent)
{
    if (windowCloseEvent == nullptr)
        throw std::exception("事件为空");

    Window::windowCloseEvent = windowCloseEvent;
}
void Window::SetWindowBeginEvent(const std::function<void()>& windowBeginEvent)
{
    if (windowBeginEvent == nullptr)
        throw std::exception("事件为空");

    Window::windowBeginEvent = windowBeginEvent;
}
void Window::SetWindowEndEvent(const std::function<void()>& windowEndEvent)
{
    if (windowEndEvent == nullptr)
        throw std::exception("事件为空");

    Window::windowEndEvent = windowEndEvent;
}
void Window::SetWindowUpdateEvent(const std::function<void()>& windowUpdateEvent)
{
    if (windowUpdateEvent == nullptr)
        throw std::exception("事件为空");

    Window::windowUpdateEvent = windowUpdateEvent;
}

void Window::Initialize()
{
    if (!glfwInit())
        throw std::exception("窗口初始化失败");

    glfwWindow = glfwCreateWindow(648, 480, "Hello Light", nullptr, nullptr);
    if (!glfwWindow)
    {
        glfwTerminate();
        throw std::exception("窗口创建失败");
    }

    Input::Initialize(glfwWindow);
}
void Window::Begin()
{
    glfwMakeContextCurrent(glfwWindow);

    //启动窗口
    windowBeginEvent();

    do
    {
        while (!glfwWindowShouldClose(glfwWindow))
        {
            Time::Update();
            glfwPollEvents();
            Input::Update();

            //更新窗口
            windowUpdateEvent();

            glfwSwapBuffers(glfwWindow);
        }

        //确定关闭
        glfwSetWindowShouldClose(glfwWindow, windowCloseEvent() ? GLFW_TRUE : GLFW_FALSE);
    }
    while (!glfwWindowShouldClose(glfwWindow));

    //关闭窗口
    windowEndEvent();

    glfwDestroyWindow(glfwWindow);
    
    glfwTerminate();
}
void Window::End()
{
    glfwSetWindowShouldClose(glfwWindow,GLFW_TRUE);
}
