#pragma once

#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamWindow/Runtime/Window.h"

namespace Gleam
{
    class WindowSystem : public System<PreUpdateSystem>, public ISystemGroup
    {
        void Update() override;
    };
    Gleam_MakeTypeWithID(WindowSystem, "")
    {
        transferrer.TransferField("mousePosition", Window::mousePosition);
        transferrer.TransferField("mouseScrollDelta", Window::mouseScrollDelta);
        transferrer.TransferField("mouseButtonStates", Window::mouseButtonStates);
        transferrer.TransferField("keyboardStates", Window::keyboardStates);
    }
    Gleam_MakeRuntimeSystem(WindowSystem)
}