#include "LogicSystem.h"

#include "AssetSystem.h"
#include "GleamWindow/Runtime/System/InputSystem.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"
#include "GleamMassSpring/Runtime/Entity/Particle.h"
#include "GleamMassSpring/Runtime/Entity/Archetype.h"
#include "GleamMath/Runtime/LinearAlgebra/MatrixMath.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/System/InspectorWindow.h"
#endif

using namespace Gleam;

void LogicSystem::OnMoveParticle()
{
    static float lastDrag = 0;
    if (GlobalInputSystem->GetMouseButtonDown(MouseButton::Left))
    {
        fixedParticle = coveringParticle;
        if (World::GetEntityInfoAllocator().HasEntity(fixedParticle))
        {
            Particle& particle = World::GetEntityAllocator().GetComponent<Particle>(fixedParticle);
            lastDrag = particle.drag;
            particle.drag = 1;
        }
#ifdef GleamEngineEditor
        GlobalInspectorWindow->SetMajorTarget(fixedParticle);
#endif
    }
    else if (GlobalInputSystem->GetMouseButtonUp(MouseButton::Left))
    {
        if (World::GetEntityInfoAllocator().HasEntity(fixedParticle))
        {
            World::GetEntityAllocator().GetComponent<Particle>(fixedParticle).drag = lastDrag;
        }
        fixedParticle = Entity::Null;
    }
}
void LogicSystem::OnCreateParticle() const
{
    if (GlobalInputSystem->GetMouseButtonDown(MouseButton::Left))
    {
        // ReSharper disable once CppDeclaratorNeverUsed
        Entity entity = Physics::AddParticle(mousePositionWS, drag, mass);
#ifdef GleamEngineEditor
        GlobalInspectorWindow->SetMajorTarget(entity);
#endif
    }
}

std::vector<Entity> lines;
void LogicSystem::OnDeleteParticle()
{
    if (GlobalInputSystem->GetMouseButtonDown(MouseButton::Left) && coveringParticle != Entity::Null)
    {
        lines.clear();
        World::GetView<Spring>().Each([this](const Entity entity, Spring& springPhysics)
        {
            if (springPhysics.particleA == coveringParticle || springPhysics.particleB == coveringParticle)
                lines.push_back(entity);
        });
        for (Entity entity : lines)
            World::RemoveSceneEntityAsync(entity);
        World::RemoveSceneEntityAsync(coveringParticle);
    }
}
void LogicSystem::OnCreateSpring()
{
    if (springParticleA == Entity::Null)
    {
        if (GlobalInputSystem->GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringParticle != Entity::Null)
            {
                springParticleA = coveringParticle;
                tempLine = World::AddSceneEntity(LineRendererArchetype);
            }
        }
    }
    else
    {
        if (GlobalInputSystem->GetMouseButtonDown(MouseButton::Left))
        {
            if (coveringParticle != Entity::Null && coveringParticle != springParticleA)
                Physics::AddSpring(springParticleA, coveringParticle, elasticity);

            springParticleA = Entity::Null;
            World::RemoveSceneEntityAsync(tempLine);
        }
    }

    if (tempLine != Entity::Null)
    {
        Particle particleA = World::GetEntityAllocator().GetComponent<Particle>(springParticleA);
        World::GetEntityAllocator().GetComponent<LinesMesh>(tempLine).lines = {
            Segment{particleA.position, mousePositionWS}
        };
    }
}
void LogicSystem::Update()
{
    //获取鼠标位置
    ScreenToWorld screenToWorld = World::GetEntityAllocator().GetComponent<ScreenToWorld>(GlobalAssetSystem->GetCameraEntity());
    mousePositionWS = float3(mul(screenToWorld.value, float4(GlobalInputSystem->GetMousePosition(), 0, 1)).xy, 1);
    //获取当前鼠标覆盖的顶点
    coveringParticle = Entity::Null;
    World::GetView<Particle>().Each([this](const Entity entity, const Particle& particle)
    {
        if (distance(particle.position, mousePositionWS) < 2)
            coveringParticle = entity;
    });

    switch (editMode)
    {
    case EditMode::MoveParticle: OnMoveParticle();
        break;
    case EditMode::CreateParticle: OnCreateParticle();
        break;
    case EditMode::DeleteParticle: OnDeleteParticle();
        break;
    case EditMode::CreateSpring: OnCreateSpring();
        break;
    }

    GlobalTimeSystem->SetTimeScale(simulatedSpeed);

    for (Entity collider : GlobalAssetSystem->colliders)
        World::GetEntityAllocator().SetComponents(collider, Collider{colliderFriction, colliderElasticity});
}

void PhysicsLogicSystem::Update()
{
    if (World::GetEntityInfoAllocator().HasEntity(GlobalLogicSystem->fixedParticle))
    {
        Particle& particle = World::GetEntityAllocator().GetComponent<Particle>(GlobalLogicSystem->fixedParticle);
        particle.position = GlobalLogicSystem->mousePositionWS;
    }

    if (GlobalLogicSystem->test)
    {
        Particle& particle = World::GetEntityAllocator().GetComponent<Particle>(GlobalAssetSystem->centerParticle);
        particle.drag = 1;
        float time = GlobalTimeSystem->GetTime() * 20;
        particle.position = float3{std::cos(time) * 50, std::sin(time) * 60, 1};
    }

    World::GetView<Particle>().Each([](Particle& particle)
    {
        particle.position.z = 1;
        particle.lastPosition.z = 1;
    });
}