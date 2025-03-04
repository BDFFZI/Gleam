#include "ParticleSelectionSystem.h"

#include "GleamECS/Runtime/View.h"
#include "GleamEngine/Editor/System/InspectorWindow.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"
#include "GleamRendering/Editor/Handles.h"
#include "GleamRendering/Editor/System/WorldWindow.h"
#include "GleamRendering/Runtime/Component/Camera.h"
#include "GleamWindow/Runtime/System/InputSystem.h"

namespace Gleam
{
    void ParticleSelectionSystem::Update()
    {
        InputSystem& inputSystem = GlobalWorldWindow.GetSceneInputSystem();

        float4x4 worldToClip = World::GetComponent<WorldToClip>(GlobalWorldWindow.GetSceneCamera()).value;
        float4x4 screenToClip = World::GetComponent<ScreenToClip>(GlobalWorldWindow.GetSceneCamera()).value;
        float2 mousePositionNDC = mul(screenToClip, float4(inputSystem.GetMousePosition(), 0, 1)).xy;

        optionalEntity = Entity::Null;
        optionalEntityZ = 1;
        View<Particle>::Each([this,&worldToClip,mousePositionNDC](const Entity entity, const Particle& point)
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
        if (inputSystem.GetMouseButtonUp(MouseButton::Left) && !ImGuizmo::IsUsingAny())
        {
            if (optionalEntity != Entity::Null)
                inspectingEntity = optionalEntity;
            if (!GlobalInspectorWindow.GetTarget().has_value())
                GlobalInspectorWindow.SetTarget(InspectorTarget{inspectingEntity});
            else if (GlobalInspectorWindow.GetTarget().value().type == typeid(Entity))
            {
                Entity entity = *static_cast<Entity*>(GlobalInspectorWindow.GetTarget().value().data);
                if (!World::HasEntity(entity) || World::HasComponent<Particle>(entity))
                    GlobalInspectorWindow.SetTarget(InspectorTarget{inspectingEntity});
            }
        }
    }
}