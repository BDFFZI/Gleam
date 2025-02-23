#pragma once
#include "GleamECS/Runtime/System.h"
#include "GleamUI/Runtime/UISystem.h"

namespace Gleam
{
    class HandlesSystem : public System
    {
    public:
        static HandlesSystem& GetInstance();

    private:
        Gleam_Engine_Friend

        HandlesSystem(): System(UISystem, MinOrder, DefaultOrder)
        {
        }

        void Start() override;
        void Update() override;
    };
}