#pragma once
#include "../Systems/UISystem.h"
#include "LightECS/Runtime/_Concept.hpp"
#include "LightECS/Runtime/_Template.hpp"
#include "LightReflection/Runtime/Serialization/Serializer.hpp"

namespace Light
{
    struct InspectorWindow : SystemT<UISystemGroup>
    {
        inline static Entity target;

        static void Update();
    };
}
