#pragma once
#include "GleamMath/Runtime/Geometry/2D/Rectangle.h"
#include "GleamWindow/Runtime/Library/InputEnum.h"

namespace Gleam
{
    struct Input
    {
        bool GetIsFocus() const
        {
            return isFocus;
        }
        bool GetMouseButtonDown(MouseButton mouseButton) const
        {
            const uint8_t index = static_cast<uint8_t>(mouseButton);
            return mouseButtonStates[index][0] == false &&
                mouseButtonStates[index][1] == true;
        }
        bool GetMouseButton(MouseButton mouseButton) const
        {
            const uint8_t index = static_cast<uint8_t>(mouseButton);
            return mouseButtonStates[index][0] == true &&
                mouseButtonStates[index][1] == true;
        }
        bool GetMouseButtonUp(MouseButton mouseButton) const
        {
            const uint8_t index = static_cast<uint8_t>(mouseButton);
            return mouseButtonStates[index][0] == true &&
                mouseButtonStates[index][1] == false;
        }
        bool GetKeyDown(KeyCode keyCode) const
        {
            const uint16_t index = static_cast<uint16_t>(keyCode);
            return keyboardStates[index][0] == false &&
                keyboardStates[index][1] == true;
        }
        bool GetKey(KeyCode keyCode) const
        {
            const uint16_t index = static_cast<uint16_t>(keyCode);
            return keyboardStates[index][0] == true &&
                keyboardStates[index][1] == true;
        }
        bool GetKeyUp(KeyCode keyCode) const
        {
            const uint16_t index = static_cast<uint16_t>(keyCode);
            return keyboardStates[index][0] == true &&
                keyboardStates[index][1] == false;
        }
        float2 GetMousePosition() const { return mousePosition[1]; }
        float2 GetMouseMoveDelta() const { return mousePosition[1] - mousePosition[0]; }
        float2 GetMouseScrollDelta() const { return mouseScrollDelta; }
        void SetFocusArea(const Rectangle rect) { focusArea = rect; }

    private:
        Gleam_MakeType_Friend
        friend class InputSystem;

        Rectangle focusArea = {0, std::numeric_limits<float>::max()};
        bool isFocus = true;
        bool mouseButtonStates[3][2] = {};
        bool keyboardStates[349][2] = {};
        float2 mousePosition[2] = {};
        float2 mouseScrollDelta = {};
    };
    Gleam_MakeType(Input)
    {
        Gleam_MakeType_AddField(focusArea);
        Gleam_MakeType_AddField(isFocus);
        transferrer.TransferField("mousePosition", value.mousePosition[1]);
        Gleam_MakeType_AddField(mouseScrollDelta);
    }
}