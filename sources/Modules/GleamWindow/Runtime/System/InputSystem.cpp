#include "InputSystem.h"

#include "GleamMath/Runtime/Geometry/Geometry.h"
#include "GleamWindow/Runtime/Cursor.h"
#include "GleamWindow/Runtime/InputEnum.h"
#include "GleamWindow/Runtime/Window.h"

using namespace Gleam;

void InputSystem::Update()
{
    //鼠标按钮
    for (uint8_t i = 0; i < MouseButtonCount; i++)
    {
        bool* mouseButtonState = mouseButtonStates[i];
        mouseButtonState[0] = mouseButtonState[1];
        mouseButtonState[1] = Window::GetMouseButtonState(static_cast<MouseButton>(i));
    }

    //键盘按钮
    for (uint16_t i = 0; i < KeyCodeCount; i++)
    {
        bool* keyboardState = keyboardStates[i];
        keyboardState[0] = keyboardState[1];
        keyboardState[1] = Window::GetKeyboardState(static_cast<KeyCode>(i));
    }

    //鼠标位置
    mousePosition[0] = Cursor::GetLockState() ? Cursor::GetLockPosition() - focusArea.min : mousePosition[1];
    mousePosition[1] = Window::GetMousePosition() - focusArea.min;

    //鼠标滚轮
    mouseScrollDelta = Window::GetMouseScrollDelta();

    //更新焦点状态
    if (Window::GetIsMouseDown())
        isFocus = Geometry::Contains(focusArea, Window::GetMousePosition());

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