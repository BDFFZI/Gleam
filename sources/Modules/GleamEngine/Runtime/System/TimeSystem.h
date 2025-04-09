#pragma once

#include "UpdateSystem.h"
#include "GleamECS/Runtime/View/View.h"
#include "GleamEngine/Runtime/Entity/Archetype.h"
#include "GleamEngine/Runtime/Entity/Time.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamUtility/Runtime/Timer.h"

namespace Gleam
{
    class TimeSystem : public SystemT<PreUpdateSystem>
    {
    public:
        Time& GetMainTime() const
        {
            return entities->GetComponent<Time>(mainTime);
        }

    private:
        EntityAllocator* entities = nullptr;
        View<Time> view = {};
        Entity mainTime = {};

        void Start() override;
        void Update() override;
    };
    Gleam_MakeSystem(TimeSystem)
}