#pragma once

#include "WindowSystem.h"
#include "LightECS/Runtime/System.h"
#include "LightMath/Runtime/LinearAlgebra/VectorMath.h"
#include "LightMath/Runtime/Geometry/Rect.h"
#include "LightWindow/Runtime/InputEnum.h"

namespace Light
{
    /**
     * 每帧将GLFW传入的用户输入解析成Light所用的输入格式
     */
    class InputSystem : public System
    {
    public:
        InputSystem(const std::string_view name = ""): System(WindowSystem, MiddleOrder, RightOrder)
        {
            this->GetName() = name;
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
        Light_MakeType_Friend

        Rect focusArea = {0, 0, std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
        bool isFocus = true;
        bool mouseButtonStates[3][2] = {};
        bool keyboardStates[349][2] = {};
        float2 mousePosition[2] = {};
        float2 mouseScrollDelta = {};

        void Update() override;
    };
    Light_MakeSystemInstance(InputSystem)

    Light_MakeType(InputSystem_T, "")
    {
        Light_MakeType_AddField(focusArea);
        Light_MakeType_AddField(isFocus);
        transferrer.TransferField("mousePosition", value.mousePosition[1]);
        Light_MakeType_AddField(mouseScrollDelta);
    }
}