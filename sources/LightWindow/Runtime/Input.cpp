#include "Input.h"

#include "Window.h"

using namespace Light;

bool Input::GetMouseButtonDown(MouseButton mouseButton)
{
    const int index = static_cast<int>(mouseButton);
    return mouseButtonState[index][0] == false &&
        mouseButtonState[index][1] == true;
}
bool Input::GetMouseButton(MouseButton mouseButton)
{
    const int index = static_cast<int>(mouseButton);
    return mouseButtonState[index][0] == true &&
        mouseButtonState[index][1] == true;
}
bool Input::GetMouseButtonUp(MouseButton mouseButton)
{
    const int index = static_cast<int>(mouseButton);
    return mouseButtonState[index][0] == true &&
        mouseButtonState[index][1] == false;
}
bool Input::GetKeyDown(KeyCode keyCode)
{
    const int index = static_cast<int>(keyCode);
    return keyboardState[index][0] == false &&
        keyboardState[index][1] == true;
}
bool Input::GetKey(KeyCode keyCode)
{
    const int index = static_cast<int>(keyCode);
    return keyboardState[index][0] == true &&
        keyboardState[index][1] == true;
}
bool Input::GetKeyUp(KeyCode keyCode)
{
    const int index = static_cast<int>(keyCode);
    return keyboardState[index][0] == true &&
        keyboardState[index][1] == false;
}

void Input::GlfwWindowFocusCallback(GLFWwindow* window, const int focused)
{
    if (inputHandlers.top().glfwWindowFocusCallback)
        inputHandlers.top().glfwWindowFocusCallback(window, focused);
}
void Input::GlfwCursorEnterCallback(GLFWwindow* window, const int entered)
{
    if (inputHandlers.top().glfwCursorEnterCallback)
        inputHandlers.top().glfwCursorEnterCallback(window, entered);
}
void Input::GlfwCursorPosCallback(GLFWwindow* window, const double xPos, const double yPos)
{
    if (inputHandlers.top().glfwCursorPosCallback)
        inputHandlers.top().glfwCursorPosCallback(window, xPos, yPos);

    mousePosition[1] = float2{static_cast<float>(xPos), static_cast<float>(yPos)} - mouseOrigin;
}
void Input::GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
{
    if (inputHandlers.top().glfwMouseButtonCallback)
        inputHandlers.top().glfwMouseButtonCallback(window, button, action, mods);

    mouseButtonState[button][2] = action != GLFW_RELEASE;
}
void Input::GlfwScrollCallback(GLFWwindow* window, const double xOffset, const double yOffset)
{
    if (inputHandlers.top().glfwScrollCallback)
        inputHandlers.top().glfwScrollCallback(window, xOffset, yOffset);

    mouseScrollDelta[1] = {static_cast<float>(xOffset), static_cast<float>(yOffset)};
}
void Input::GlfwKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
{
    if (inputHandlers.top().glfwKeyCallback)
        inputHandlers.top().glfwKeyCallback(window, key, scancode, action, mods);

    keyboardState[key][2] = action != GLFW_RELEASE;
}
void Input::GlfwCharCallback(GLFWwindow* window, const unsigned int codepoint)
{
    if (inputHandlers.top().glfwCharCallback)
        inputHandlers.top().glfwCharCallback(window, codepoint);
}
void Input::GlfwMonitorCallback(GLFWmonitor* monitor, const int event)
{
    if (inputHandlers.top().glfwMonitorCallback)
        inputHandlers.top().glfwMonitorCallback(monitor, event);
}

void Input::Initialize(GLFWwindow* glfwWindow)
{
    Input::glfwWindow = glfwWindow;
    
    glfwSetWindowFocusCallback(glfwWindow, GlfwWindowFocusCallback);
    glfwSetCursorEnterCallback(glfwWindow, GlfwCursorEnterCallback);
    glfwSetCursorPosCallback(glfwWindow, GlfwCursorPosCallback);
    glfwSetMouseButtonCallback(glfwWindow, GlfwMouseButtonCallback);
    glfwSetScrollCallback(glfwWindow, GlfwScrollCallback);
    glfwSetKeyCallback(glfwWindow, GlfwKeyCallback);
    glfwSetCharCallback(glfwWindow, GlfwCharCallback);
    glfwSetMonitorCallback(GlfwMonitorCallback);

    //添加一个默认输入回调，这样后续不需要总是判断是否存在
    PushInputHandler({"default"});
}
void Input::Start()
{
    
}
void Input::Update()
{
    glfwPollEvents();

    //鼠标按钮
    for (auto& state : mouseButtonState)
    {
        state[0] = state[1];
        state[1] = state[2];
    }
    //键盘按钮
    for (auto& state : keyboardState)
    {
        state[0] = state[1];
        state[1] = state[2];
    }
    //滚轮增量
    mouseScrollDelta[0] = mouseScrollDelta[1];
    mouseScrollDelta[1] = 0;
    //鼠标位置
    mousePositionDelta = mousePosition[1] - mousePosition[0];
    mousePosition[0] = mousePosition[1];

    if (inputHandlers.top().inputCallback)
        inputHandlers.top().inputCallback();
}
