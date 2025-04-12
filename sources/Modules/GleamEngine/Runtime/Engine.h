#pragma once
#include <functional>
#include <map>

#include "GleamECS/Runtime/World.h"
#include "GleamECS/Runtime/System/SystemInfoAllocator.h"
#include "GleamUtility/Runtime/Macro.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Profiler.h"
#endif

namespace Gleam
{
    class GlobalSystemAllocator
    {
    public:
        template <class TSystem>
        TSystem* MakeGlobalSystem(TSystem*& globalSystemPtr)
        {
            globalSystems.emplace_back(std::make_tuple<const SystemInfo*, ISystemEvent**>(
                &SystemInfoAllocator::CreateOrGetSystemInfo<TSystem>(), reinterpret_cast<ISystemEvent**>(&globalSystemPtr)
            ));
            return nullptr;
        }
        void AddGlobalSystemsToWorld()
        {
            for (const auto& [system,slot] : globalSystems)
            {
                void* address = &World::AddSystem(*system);
                *slot = static_cast<ISystemEvent*>(address);
                // *slot = &World::AddSystem(*system); //注意！不能这样连写，否则取到的地址是错的，神奇的机制
            }
        }

    private:
        std::vector<std::tuple<const SystemInfo*, ISystemEvent**>> globalSystems;
    };

    class Engine
    {
    public:
        static void AddStartEvent(const std::function<void()>& event, int order = 0);
        static void AddStopEvent(const std::function<void()>& event, int order = 0);
        static void AddUpdateEvent(const std::function<void()>& event, int order = 0);
        static GlobalSystemAllocator& GetRuntimeSystems()
        {
            return runtimeSystems;
        }

        static void Start()
        {
            assert(!isStopping && "引擎尚未启动就已被关闭，请检查运行流程！");

            for (auto& event : startEvents | std::views::values)
                event();

            runtimeSystems.AddGlobalSystemsToWorld();

            while (!isStopping)
            {
#ifdef GleamEngineEditor
                Profiler::Begin("Update");
                World::Update();
                Profiler::End();
#else
                World::Update();
#endif

                for (auto& event : updateEvents | std::views::values)
                    event();
            }

            World::Clear();

            for (auto& event : stopEvents | std::views::values)
                event();
        }
        static void Stop();

    private:
        friend class Editor;
        friend void Editor_ReplaceRuntimeSystem();
        friend void Editor_PlayOrStopEngine();

        static inline GlobalSystemAllocator runtimeSystems;
        static inline std::multimap<int, std::function<void()>> startEvents;
        static inline std::multimap<int, std::function<void()>> updateEvents;
        static inline std::multimap<int, std::function<void()>> stopEvents;
        inline static bool isStopping = false;
    };

    ///利用如下宏实现关系到程序整个运行周期的事件，如库初始化。
    ///这些事件与System中的Start和Stop不同，System在程序运行中可能多次Start和Stop，
    ///且System::Stop在实体回收前执行，如果在Stop逆初始化库，这可能导致实体中需要该库的数据可能无法正常回收。
#define Gleam_MakeEngineEvent(eventType, eventName, order) \
inline void eventName();\
constexpr int eventName##Order = order;\
Gleam_MakeInitEvent(){Engine::Add##eventType##Event(eventName,eventName##Order);}\
inline void eventName()
#define Gleam_MakeEngineStartEvent(eventName, order) Gleam_MakeEngineEvent(Start, eventName, order)
#define Gleam_MakeEngineStopEvent(eventName, order) Gleam_MakeEngineEvent(Stop, eventName, order)
#define Gleam_MakeEngineUpdateEvent(eventName, order) Gleam_MakeEngineEvent(Update, eventName, order)

#define Gleam_AddEngineEvent(eventType, eventName, order) \
constexpr int eventName##Order = order;\
Gleam_MakeInitEvent(){Engine::Add##eventType##Event(eventName,eventName##Order);}
#define Gleam_AddEngineStartEvent(eventName, order) Gleam_AddEngineEvent(Start, eventName, order)
#define Gleam_AddEngineStopEvent(eventName, order) Gleam_AddEngineEvent(Stop, eventName, order)
#define Gleam_AddEngineUpdateEvent(eventName, order) Gleam_AddEngineEvent(Update, eventName, order)

#define Gleam_Main \
inline int main()\
{\
Gleam::Engine::Start();\
return 0;\
}

#define Gleam_MakeRuntimeSystem(type) inline type* Global##type = ::Gleam::Engine::GetRuntimeSystems().MakeGlobalSystem<type>(Global##type);
}