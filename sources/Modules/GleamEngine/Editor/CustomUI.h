#pragma once

#include "GleamECS/Runtime/Entity/Archetype.h"
#include "GleamEngine/Runtime/Entity/Transform.h"

namespace Gleam
{
    inline Entity InspectorWindowUI_Entity_Target = Entity::Null;
    void InspectorWindowUI_Entity(Entity entity);
    void InspectorWindowUI_LocalTransform(LocalTransform& localTransform);
}