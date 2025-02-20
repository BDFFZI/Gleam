#include "PointSelectionSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamMath/Runtime/Geometry/3D/Point.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Editor/Handles.h"
#include "GleamRendering/Editor/System/SceneWindow.h"
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    void PointSelectionSystem::Update()
    {
        InputSystem_T& inputSystem = SceneWindow.GetSceneInputSystem();

        float4x4 worldToClip = World::GetComponent<WorldToClip>(SceneWindow.GetSceneCamera()).value;
        float4x4 screenToClip = World::GetComponent<ScreenToClip>(SceneWindow.GetSceneCamera()).value;
        float2 mousePositionNDC = mul(screenToClip, float4(inputSystem.GetMousePosition(), 0, 1)).xy;

        optionalEntity = Entity::Null;
        optionalEntityZ = 1;
        View<Point>::Each([this,&worldToClip,mousePositionNDC](const Entity entity, const Point& point)
        {
            float4 pointPositionCS = mul(worldToClip, float4(point.position, 1));
            float3 pointPositionNDC = pointPositionCS.xyz / pointPositionCS.w;

            if (pointPositionNDC.z > 0 && pointPositionNDC.z < 1 && distance(mousePositionNDC, pointPositionNDC.xy) < 0.03f)
            {
                if (pointPositionNDC.z < optionalEntityZ)
                {
                    optionalEntity = entity;
                    optionalEntityZ = pointPositionNDC.z;
                }
            }
        });

        //取消选择
        if (inputSystem.GetMouseButton(MouseButton::Left) && !ImGuizmo::IsUsingAny())
            InspectorWindow.SetTarget(optionalEntity);
    }
}