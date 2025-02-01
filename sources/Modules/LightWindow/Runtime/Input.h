#pragma once

#include "Window.h"
#include "LightECS/Runtime/System.h"
#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"
#include "LightMath/Runtime/Geometry/Rect.h"

namespace Light
{
    /**
     * 每帧将GLFW传入的用户输入解析成Light所用的输入格式
     */
    class Input : public System
    {
    public:
        Input(): System(Window, MiddleOrder, RightOrder)
        {
        }

        bool GetMouseButtonDown(MouseButton mouseButton) const;
        bool GetMouseButton(MouseButton mouseButton) const;
        bool GetMouseButtonUp(MouseButton mouseButton) const;
        bool GetKeyDown(KeyCode keyCode) const;
        bool GetKey(KeyCode keyCode) const;
        bool GetKeyUp(KeyCode keyCode) const;
        float2 GetMouseScrollDelta() const { return mouseScrollDelta; }
        float2 GetMousePosition() const { return mousePosition[1]; }
        float2 GetMouseMoveDelta() const { return mousePosition[1] - mousePosition[0]; }
        void SetFocusArea(const Rect rect) { focusArea = rect; }

    private:
        Rect focusArea = {0, 0, std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        bool isFocus = true;
        bool mouseButtonStates[3][2] = {};
        bool keyboardStates[349][2] = {};
        float2 mousePosition[2] = {};
        float2 mouseScrollDelta = {};

        void Update() override;
    };
    Light_MakeSystem(Input)
}