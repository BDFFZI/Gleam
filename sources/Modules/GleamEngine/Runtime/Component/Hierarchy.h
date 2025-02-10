#pragma once
#include <unordered_set>

#include "GleamECS/Runtime/Entity.h"
#include "GleamReflection/Runtime/Type.h"

namespace Gleam
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

    Gleam_MakeType(Parent, "5814D8DB-67FC-454F-8527-32DE6816A98D")
    {
        Gleam_MakeType_AddField(current);
    }
}