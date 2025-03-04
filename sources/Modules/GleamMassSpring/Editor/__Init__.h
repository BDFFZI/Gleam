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
    
    Gleam_MakeSceneUI(Particle, SceneUI_Particle)
}