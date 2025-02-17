#include "PointSelector.h"

#include <iostream>

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamMath/Runtime/Geometry/Basic/Point.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Editor/SceneWindow.h"
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    void PointSelector::Update()
    {
        InputSystem_T& inputSystem = SceneWindow.GetSceneInputSystem();

        float4x4 worldToClip = World::GetComponent<WorldToClip>(SceneWindow.GetSceneCamera()).value;
        float4x4 screenToClip = World::GetComponent<ScreenToClip>(SceneWindow.GetSceneCamera()).value;
        float2 mousePositionNDC = mul(screenToClip, float4(inputSystem.GetMousePosition(), 0, 1)).xy;

        optionalEntities.clear();
        View<Point>::Each([this,&worldToClip,mousePositionNDC](const Entity entity, const Point& point)
        {
            float4 pointPositionCS = mul(worldToClip, float4(point.position, 1));
            float2 pointPositionNDC = pointPositionCS.xy / pointPositionCS.w;

            if (distance(mousePositionNDC, pointPositionNDC) < 0.03f)
                optionalEntities.push_back(entity);
        });

        if (!optionalEntities.empty() && inputSystem.GetMouseButtonDown(MouseButton::Left))
        {
            Entity currentPoint = InspectorWindow.GetTargetEntity();
            auto it = std::ranges::find(optionalEntities, currentPoint);
            if (it == optionalEntities.end() || it + 1 == optionalEntities.end())
                InspectorWindow.SetTarget(optionalEntities[0]);
            else
                InspectorWindow.SetTarget(*(it + 1));
        }
    }
}