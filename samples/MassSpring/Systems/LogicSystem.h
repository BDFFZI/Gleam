#pragma once
#include "UISystem.h"
#include "LightECS/Runtime/_Concept.hpp"
#include "LightECS/Runtime/_Template.hpp"
#include "LightWindow/Runtime/Input.h"

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

        inline static float2 mousePositionWS = 0;
        inline static Entity coveringPoint = Entity::Null;

        inline static Entity fixedPoint = Entity::Null;
        inline static Entity springPointA = Entity::Null;
        inline static Entity tempLinePointB = Entity::Null;
        inline static Entity tempLine = Entity::Null;

        static void Start();
        static void Stop();
        static void Update();
        static void FixedUpdate();

    private:
        inline static InputHandler inputHandler = {"LogicSystemInputHandler"};

        static void UpdateUI();
        static void OnMovePoint();
        static void OnCreatePoint();
        static void OnDeletePoint();
        static void OnCreateSpring();
    };
}
