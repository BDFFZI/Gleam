#pragma once
#include "UISystem.h"
#include "LightECS/Runtime/_Concept.hpp"
#include "LightECS/Runtime/_Template.hpp"

namespace Light
{
    enum class EditMode:uint8_t
    {
        MovePoint,
        CreatePoint,
        DeletePoint,
        CreateSpring,
    };

    struct LogicSystem : SystemT<UISystemGroup>
    {
        inline static EditMode editMode = EditMode::MovePoint;
        inline static bool simulating = false;
        inline static Entity selectingPoint = Entity::Null;
        inline static float2 mousePositionWS = 0;

        static void Start();
        static void Stop();
        static void Update();
        static void FixedUpdate();

    private:
        static void UpdateUI();
        static void OnMovePoint();
        static void OnCreatePoint();
        static void OnDeletePoint();
        static void OnCreateSpring();
    };
}
