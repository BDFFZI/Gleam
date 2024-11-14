#pragma once
#include <map>
#include <set>

#include "System.hpp"
#include "LightECS/Runtime/EntitySet.h"

namespace Light
{
    class World2
    {
        using Function = void(*)();
        struct SystemInfo
        {
            const type_info* type;
            int order;
            Function start;
            Function update;
            Function stop;
        };

    public:
   

    private:
        World entitySet;
        std::multiset<SystemInfo> systems;
    };
}
