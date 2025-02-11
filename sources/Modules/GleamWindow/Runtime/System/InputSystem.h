#pragma once

#include "WindowSystem.h"
#include "GleamECS/Runtime/System.h"
#include "GleamMath/Runtime/LinearAlgebra/VectorMath.h"
#include "GleamMath/Runtime/Geometry/Rect.h"
#include "GleamWindow/Runtime/InputEnum.h"

namespace Gleam
{
    /**
     * 每帧将GLFW传入的用户输入解析成Gleam所用的输入格式
     */
    class InputSystem : public System
    {
    public:
        InputSystem(): System(WindowSystem)
        {
        }

        bool GetMouseButtonDown(MouseButton mouseButton) const;
        bool GetMouseButton(MouseButton mouseButton) const;
        bool GetMouseButtonUp(MouseButton mouseButton) const;
        bool GetKeyDown(KeyCode keyCode) const;
        bool GetKey(KeyCode keyCode) const;
        bool GetKeyUp(KeyCode keyCode) const;
        float2 GetMousePosition() const { return mousePosition[1]; }
        float2 GetMouseMoveDelta() const { return mousePosition[1] - mousePosition[0]; }
        float2 GetMouseScrollDelta() const { return mouseScrollDelta; }
        void SetFocusArea(const Rect rect) { focusArea = rect; }

    private:
        Gleam_MakeType_Friend

        Rect focusArea = {0, 0, std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        bool isFocus = true;
        bool mouseButtonStates[3][2] = {};
        bool keyboardStates[349][2] = {};
        float2 mousePosition[2] = {};
        float2 mouseScrollDelta = {};

        void Update() override;
    };
    Gleam_MakeGlobalSystem(InputSystem)

    Gleam_MakeType(InputSystem_T, "")
    {
        Gleam_MakeType_AddField(focusArea);
        Gleam_MakeType_AddField(isFocus);
        transferrer.TransferField("mousePosition", value.mousePosition[1]);
        Gleam_MakeType_AddField(mouseScrollDelta);
    }
}