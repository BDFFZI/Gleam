#pragma once

#include "GleamEngine/Runtime/Component/Transform.h"
#include "System/InspectorWindow.h"

namespace Gleam
{
    inline Entity InspectorWindowUI_Entity_Target = Entity::Null;
    void InspectorWindowUI_Entity(Entity entity);
    void InspectorWindowUI_LocalTransform(LocalTransform& localTransform);
}