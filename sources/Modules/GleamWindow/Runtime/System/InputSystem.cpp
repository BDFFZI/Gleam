#include "InputSystem.h"

#include "GleamMath/Runtime/Geometry/Geometry.h"
#include "GleamWindow/Runtime/Library/Cursor.h"
#include "GleamWindow/Runtime/Library/InputEnum.h"
#include "GleamWindow/Runtime/Library/Window.h"

using namespace Gleam;


void InputSystem::Start()
{
    entities = &World::GetCurrentWorld().GetEntityAllocator();
    view = {*entities};
    defaultInput = entities->AddEntity(InputData);
}
void InputSystem::Update()
{
    view.Each([](Input& input)
    {
        //鼠标按钮
        for (uint8_t i = 0; i < MouseButtonCount; i++)
        {
            bool* mouseButtonState = input.mouseButtonStates[i];
            mouseButtonState[0] = mouseButtonState[1];
            mouseButtonState[1] = Window::GetMouseButtonState(static_cast<MouseButton>(i));
        }

        //键盘按钮
        for (uint16_t i = 0; i < KeyCodeCount; i++)
        {
            bool* keyboardState = input.keyboardStates[i];
            keyboardState[0] = keyboardState[1];
            keyboardState[1] = Window::GetKeyboardState(static_cast<KeyCode>(i));
        }

        //鼠标位置
        input.mousePosition[0] = Cursor::GetLockState() ? Cursor::GetLockPosition() - input.focusArea.min : input.mousePosition[1];
        input.mousePosition[1] = Window::GetMousePosition() - input.focusArea.min;

        //鼠标滚轮
        input.mouseScrollDelta = Window::GetMouseScrollDelta();

        //更新焦点状态
        if (Window::GetIsMouseDown())
            input.isFocus = Geometry::Contains(input.focusArea, Window::GetMousePosition());

        //根据焦点状态调整输入信息
        if (input.isFocus == false)
        {
            //鼠标按钮
            for (auto mouseButtonState : input.mouseButtonStates)
                mouseButtonState[1] = false;
            //键盘按钮
            for (auto keyboardState : input.keyboardStates)
                keyboardState[1] = false;
            //鼠标位置
            input.mousePosition[1] = input.mousePosition[0];
        }
    });
}