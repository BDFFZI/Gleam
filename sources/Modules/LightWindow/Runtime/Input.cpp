#include "Input.h"

#include "Cursor.h"
#include "Window.h"

using namespace Light;

bool Input::GetMouseButtonDown(MouseButton mouseButton) const
{
    const uint8_t index = static_cast<uint8_t>(mouseButton);
    return mouseButtonStates[index][0] == false &&
        mouseButtonStates[index][1] == true;
}
bool Input::GetMouseButton(MouseButton mouseButton) const
{
    const uint8_t index = static_cast<uint8_t>(mouseButton);
    return mouseButtonStates[index][0] == true &&
        mouseButtonStates[index][1] == true;
}
bool Input::GetMouseButtonUp(MouseButton mouseButton) const
{
    const uint8_t index = static_cast<uint8_t>(mouseButton);
    return mouseButtonStates[index][0] == true &&
        mouseButtonStates[index][1] == false;
}
bool Input::GetKeyDown(KeyCode keyCode) const
{
    const uint16_t index = static_cast<uint16_t>(keyCode);
    return keyboardStates[index][0] == false &&
        keyboardStates[index][1] == true;
}
bool Input::GetKey(KeyCode keyCode) const
{
    const uint16_t index = static_cast<uint16_t>(keyCode);
    return keyboardStates[index][0] == true &&
        keyboardStates[index][1] == true;
}
bool Input::GetKeyUp(KeyCode keyCode) const
{
    const uint16_t index = static_cast<uint16_t>(keyCode);
    return keyboardStates[index][0] == true &&
        keyboardStates[index][1] == false;
}

void Input::Update()
{
    //鼠标按钮
    for (uint8_t i = 0; i < MouseButtonCount; i++)
    {
        bool* mouseButtonState = mouseButtonStates[i];
        mouseButtonState[0] = mouseButtonState[1];
        mouseButtonState[1] = Window->GetMouseButtonState(static_cast<MouseButton>(i));
    }

    //键盘按钮
    for (uint16_t i = 0; i < KeyCodeCount; i++)
    {
        bool* keyboardState = keyboardStates[i];
        keyboardState[0] = keyboardState[1];
        keyboardState[1] = Window->GetKeyboardState(static_cast<KeyCode>(i));
    }

    //鼠标位置
    mousePosition[0] = Cursor->GetLockState() ? Cursor->GetLockPosition() - focusArea.GetPosition() : mousePosition[1];
    mousePosition[1] = Window->GetMousePosition() - focusArea.GetPosition();

    //更新焦点状态
    if (Window->IsMouseDown())
        isFocus = focusArea.Contains(Window->GetMousePosition());

    //根据焦点状态调整输入信息
    if (isFocus == false)
    {
        //鼠标按钮
        for (auto mouseButtonState : mouseButtonStates)
            mouseButtonState[1] = false;
        //键盘按钮
        for (auto keyboardState : keyboardStates)
            keyboardState[1] = false;
        //鼠标位置
        mousePosition[1] = mousePosition[0];
    }
}