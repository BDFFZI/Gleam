#pragma once
#include "CustomUI.h"
#include "GleamMassSpring/Runtime/Component/Particle.h"
#include "System/MassSpringRenderingSystem.h"
#include "System/ParticleSelectionSystem.h"

namespace Gleam
{
    Gleam_AddEditorSystems(
        GlobalParticleSelectionSystem,
        GlobalMassSpringRenderingSystem
    )
    
    Gleam_AddSceneWindowUI(Particle, SceneWindowUI_Particle)
}