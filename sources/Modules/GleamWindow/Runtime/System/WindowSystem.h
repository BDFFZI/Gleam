#pragma once

#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"
#include "GleamMath/Runtime/LinearAlgebra/Vector.h"
#include "GleamWindow/Runtime/Window.h"

namespace Gleam
{
    class WindowSystem : public SystemGroup
    {
    public:
        WindowSystem(): SystemGroup(PreUpdateSystem)
        {
        }

    private:
        void Update() override;
    };
    Gleam_MakeGlobalSystem(WindowSystem)

    Gleam_MakeType(WindowSystem_T, "")
    {
        transferrer.TransferField("mousePosition", Window::mousePosition);
        transferrer.TransferField("mouseScrollDelta", Window::mouseScrollDelta);
        transferrer.TransferField("mouseButtonStates", Window::mouseButtonStates);
        transferrer.TransferField("keyboardStates", Window::keyboardStates);
    }
}