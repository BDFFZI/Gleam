#pragma once

#include "CursorSystem.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMath/Runtime/Geometry/2D/Rectangle.h"
#include "GleamWindow/Runtime/InputEnum.h"

namespace Gleam
{
    /**
     * 每帧将GLFW传入的用户输入解析成Gleam所用的输入格式
     */
    class InputSystem : public System<CursorSystem, SystemRelation::Before>
    {
    public:
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

        Rectangle focusArea = {0, std::numeric_limits<float>::max()};
        bool isFocus = true;
        bool mouseButtonStates[3][2] = {};
        bool keyboardStates[349][2] = {};
        float2 mousePosition[2] = {};
        float2 mouseScrollDelta = {};

        void Update() override;
    };
    Gleam_MakeType(InputSystem)
    {
        Gleam_MakeType_AddField(focusArea);
        Gleam_MakeType_AddField(isFocus);
        transferrer.TransferField("mousePosition", value.mousePosition[1]);
        Gleam_MakeType_AddField(mouseScrollDelta);
    }

    Gleam_MakeRuntimeSystem(InputSystem)
}