#include "Input.h"

using namespace Light;


void Input::PushInputHandler(const InputHandler& inputHandler)
{
    inputHandlers.push(inputHandler);
}
void Input::PopInputHandler(const InputHandler& inputHandler)
{
    if (inputHandlers.top().name != inputHandler.name)
        throw std::exception("输入回调出栈顺序异常");

    inputHandlers.pop();
}

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
float2 Input::GetMouseScrollDelta()
{
    return mouseScrollDelta[0];
}
float2 Input::GetMousePosition()
{
    return mousePosition[0];
}
float2 Input::GetMouseMoveDelta()
{
    return mousePositionDelta;
}

void Input::Initialize(GLFWwindow* glfwWindow)
{
    Input::glfwWindow = glfwWindow;
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        keyboardState[key][2] = action != GLFW_RELEASE;
    });
    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, int mods)
    {
        mouseButtonState[button][2] = action != GLFW_RELEASE;
    });
    glfwSetScrollCallback(glfwWindow, [](GLFWwindow* window, double xoffset, double yoffset)
    {
        mouseScrollDelta[1] = {static_cast<float>(xoffset), static_cast<float>(yoffset)};
    });
    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xpos, double ypos)
    {
        mousePosition[1] = {static_cast<float>(xpos), static_cast<float>(ypos)};
    });


    //添加一个默认输入回调，这样后续不需要总是判断是否存在
    inputHandlers.push(
        {
            "default",
            []
            {
            },
        });
}
void Input::Update()
{
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

    inputHandlers.top().event();
}
