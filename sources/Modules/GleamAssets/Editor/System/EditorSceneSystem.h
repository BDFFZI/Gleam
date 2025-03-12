#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamEngine/Runtime/System/UpdateSystem.h"

namespace Gleam
{
    class EditorSceneSystem : public System
    {
    public:
        EditorSceneSystem(): System(GlobalPostUpdateSystem, MaxOrder, MaxOrder)
        {
        }

    private:
        void Start() override;
        void Stop() override;
    };
    Gleam_MakeGlobalSystem(EditorSceneSystem)
}