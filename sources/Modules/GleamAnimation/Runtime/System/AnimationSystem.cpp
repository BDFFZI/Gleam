#include "AnimationSystem.h"

#include "GleamAnimation/Runtime/Entity/Animation.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Runtime/System/TimeSystem.h"

namespace Gleam
{
    void AnimationSystem::Update()
    {
        World::GetView<Animation>().Each([](const Entity entity, Animation& animationClip)
        {
            if (animationClip.clip.expired() || animationClip.isPlaying == false)
                return;

            animationClip.time += GlobalTimeSystem->GetDeltaTime();
            animationClip.clip.lock()->SampleAnimation(entity, animationClip.time);
        });
    }
}