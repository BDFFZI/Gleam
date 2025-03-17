#pragma once
#include "CustomUI.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "System/MassSpringRenderingSystem.h"
#include "System/ParticleEditingSystem.h"
#include "System/ParticleSelectionSystem.h"

namespace Gleam
{
    Gleam_AddEditorSystems(
        GlobalParticleSelectionSystem,
        GlobalMassSpringRenderingSystem,
    )

    Gleam_AddEditorOnlySystems(
        GlobalParticleEditingSystem
    )

    Gleam_AddSceneWindowUI(Particle, SceneWindowUI_Particle)
}