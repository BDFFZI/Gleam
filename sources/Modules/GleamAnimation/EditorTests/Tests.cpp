#include "GleamAnimation/Runtime/Entity/Animation.h"
#include "GleamEngine/Editor/Editor.h"
#include "GleamEngine/Runtime/Engine.h"
#include "GleamMath/Runtime/Geometry/3D/Sphere.h"

using namespace Gleam;

class MySystem : public System<>
{
    std::shared_ptr<AnimationClip> clip;
    
    void Start() override
    {
        clip = std::make_shared<AnimationClip>();
        clip.
        
        Entity entity = World::GetEntityAllocator().AddEntity(Sphere{}, Animation{});
        Animation& animation = World::GetEntityAllocator().GetComponent<Animation>(entity);
        animation.clip = std::
    }
};
Gleam_MakeEditorSystem(MySystem)

Gleam_Main