#pragma once
#include <unordered_set>

#include "LightECS/Runtime/Entity.h"
#include "LightReflection/Runtime/Type.h"

namespace Light
{
    struct Parent
    {
        Entity current;
        Entity previous;
    };

    struct Children
    {
        std::unordered_set<Entity> value;
    };

    Light_MakeType(Parent, "5814D8DB-67FC-454F-8527-32DE6816A98D")
    {
        Light_MakeType_AddField(current);
    }
}