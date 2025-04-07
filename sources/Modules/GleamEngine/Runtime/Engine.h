#pragma once
#include <functional>
#include <map>

#include "GleamECS/Runtime/System/SystemGroup.h"
#include "GleamECS/Runtime/World/World.h"
#include "GleamUtility/Runtime/Macro.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Profiler.h"
#endif

namespace Gleam
{
    class Engine
    {
    public:
        static World& GetDefaultWorld()
        {
            return defaultWorld;
        }
        static void AddStartEvent(const std::function<void()>& event, int order = 0);
        static void AddStopEvent(const std::function<void()>& event, int order = 0);
        static void AddUpdateEvent(const std::function<void()>& event, int order = 0);
        template <class... TSystem>
        static void AddRuntimeSystems()
        {
            runtimeSystems.insert(runtimeSystems.end(), {&System::CreateOrGetSystemInfo<TSystem>()...});
        }

        static void Start()
        {
            assert(!isStopping && "引擎尚未启动就已被关闭，请检查运行流程！");

            for (auto system : runtimeSystems)
                defaultWorld.AddSystem(*system);

            for (auto& event : startEvents | std::views::values)
                event();

            while (!isStopping)
            {
#ifdef GleamEngineEditor
                Profiler::Begin("Update");
                World::Update();
                Profiler::End();
#else
                defaultWorld.Update();
#endif

                for (auto& event : updateEvents | std::views::values)
                    event();
            }
            defaultWorld.Clear();

            for (auto& event : stopEvents | std::views::values)
                event();
        }
        static void Stop();

    private:
        friend class Editor;
        friend void Editor_InterceptRuntimeSystem();
        friend void Editor_PlayOrStopEngine();

        static inline World defaultWorld = {};
        static inline std::vector<SystemInfo*> runtimeSystems;
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

    ///将系统添加到世界，并注册到运行时系统组
#define Gleam_AddRuntimeSystems(...) Gleam_MakeInitEvent(){::Gleam::Engine::AddRuntimeSystems<__VA_ARGS__>();}
}