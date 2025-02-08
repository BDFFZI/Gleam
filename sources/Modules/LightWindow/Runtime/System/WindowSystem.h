#pragma once

#include "LightECS/Runtime/System.h"
#include "LightEngine/Runtime/System/UpdateSystem.h"
#include "LightMath/Runtime/LinearAlgebra/Vector.h"
#include "LightWindow/Runtime/Window.h"

namespace Light
{
    class WindowSystem : public SystemGroup
    {
    public:
        WindowSystem(): SystemGroup(PreUpdateSystem, LeftOrder, MiddleOrder)
        {
        }

    private:
        void Update() override;
    };
    Light_MakeSystemInstance(WindowSystem)

    Light_MakeType(WindowSystem_T, "")
    {
        transferrer.TransferField("mousePosition", Window::mousePosition);
        transferrer.TransferField("mouseScrollDelta", Window::mouseScrollDelta);
        transferrer.TransferField("mouseButtonStates", Window::mouseButtonStates);
        transferrer.TransferField("keyboardStates", Window::keyboardStates);
    }
}