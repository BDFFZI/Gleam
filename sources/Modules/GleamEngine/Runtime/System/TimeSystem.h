#pragma once

#include "UpdateSystem.h"
#include "GleamEngine/Runtime/Entity/Archetype.h"
#include "GleamEngine/Runtime/Entity/Time.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamUtility/Runtime/Timer.h"

namespace Gleam
{
    class TimeSystem : public SystemT<PreUpdateSystem>
    {
    public:
        Time& GetDefaultTime() const
        {
            return GetEntities().GetComponent<Time>(defaultTime);
        }

    private:
        Entity defaultTime = {};
        View<Time> view = {};

        void Start() override
        {
            defaultTime = GetEntities().AddEntity(TimeData);
            view = GetView<Time>();
        }
        void Update() override;
    };
    Gleam_MakeSystem(TimeSystem)
}