#pragma once
#include <map>
#include <memory>
#include <vector>

#include "LightECS/Runtime/System.h"

namespace Light
{
    class Engine
    {
    public:
        static void AddStartEvent(const std::function<void()>& event, int order = 0);
        static void AddStopEvent(const std::function<void()>& event, int order = 0);
        static std::vector<System*>& RuntimeSystems();

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
        static inline std::vector<System*> runtimeSystems;
        static inline std::multimap<int, std::function<void()>> startEvents;
        static inline std::multimap<int, std::function<void()>> stopEvents;
        inline static bool isStopping = false;
    };

    ///创建系统并非一定要调用该函数。（我甚至在考虑移除该函数，改为让用户自行创建单例）
    ///系统本质只是一种接收事件的接口，唯一的要求就是继承System。所以系统就像实体一样是可以由用户自由创建增删。
#define Light_MakeSystemInstance(systemClass) \
using systemClass##_T = systemClass;\
inline systemClass* systemClass = Light::Engine::RegisterGlobalSystem<class systemClass##>();

#include "LightUtility/Runtime/Program.h"
#define Light_AddSystems(...) Light_MakeInitEvent(){::Light::Engine::RuntimeSystems().insert(::Light::Engine::RuntimeSystems().end(),{__VA_ARGS__});}

#define Light_AddEngineEvent(eventType, eventName, order) \
    inline void eventName();\
    inline int eventName##Order = order;\
    Light_MakeInitEvent(){Engine::Add##eventType##Event(eventName,order);}\
    inline void eventName()

#define Light_AddStartEvent(eventName, order) Light_AddEngineEvent(Start, eventName, order)
#define Light_AddStopEvent(eventName, order) Light_AddEngineEvent(Stop, eventName, order)
}