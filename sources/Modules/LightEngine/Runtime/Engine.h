#pragma once
#include <memory>
#include <vector>

#include "LightECS/Runtime/System.h"

namespace Light
{
    class Engine
    {
    public:
        template <typename TSystem>
            requires std::is_base_of_v<System, TSystem>
        static TSystem* RegisterGlobalSystem()
        {
            TSystem* system = new TSystem();
            system->GetName() = typeid(TSystem).name();
            allSystems.emplace_back(system);
            return system;
        }

        static void Start();
        static void Stop();

    private:
        friend class Editor;

        static inline std::vector<std::unique_ptr<System>> allSystems;
        inline static bool isStopping = false;
        inline static bool isUpdating = true;
    };

#define Light_MakeGlobalSystem(systemClass) \
using systemClass##_T = systemClass;\
inline systemClass* systemClass = Light::Engine::RegisterGlobalSystem<class systemClass##>();

#include "LightUtility/Runtime/Program.h"
#define Light_AddSystems(...) Light_MakeInitEvent(){::Light::World::AddSystems({__VA_ARGS__});}
}