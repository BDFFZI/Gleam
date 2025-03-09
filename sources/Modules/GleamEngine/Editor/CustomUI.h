#pragma once

#include "GleamEngine/Runtime/Component/Transform.h"
#include "System/InspectorWindow.h"

namespace Gleam
{
    inline static Entity InspectorUI_Entity_Target = Entity::Null;
    void InspectorUI_Entity(Entity entity);
    void InspectorUI_LocalTransform(LocalTransform& localTransform);
}