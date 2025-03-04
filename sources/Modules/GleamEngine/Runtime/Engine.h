#pragma once
#include <functional>
#include <map>

#include "GleamECS/Runtime/System.h"
#include "GleamECS/Runtime/World.h"
#include "GleamReflection/Runtime/Type.h"
#include "GleamUtility/Runtime/Macro.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Profiler.h"
#endif

namespace Gleam
{
    class Engine
    {
    public:
        /**
         * 一个更完善的System创建函数。
         * 相比正常使用构造函数创建，该函数能自动按需设置系统名称以及注册类型信息。
         * @tparam TSystem 
         * @param name 
         * @return 
         */
        template <typename TSystem> requires std::derived_from<TSystem, System>
        static TSystem CreateSystem(const std::string_view name = "")
        {
            TSystem system = {};
            //自动设置名称
            if (!name.empty())
                system.GetName() = name;
            else if (system.GetName().empty())
            {
                std::string defaultName = std::string(typeid(TSystem).name());
                defaultName = defaultName.substr(defaultName.find_last_of(' ') + 1);
                system.GetName() = defaultName;
            }
            //自动设置Type的父类信息
            Type& type = Type::CreateOrGet<TSystem>();
            if (!type.GetParent().has_value())
                type.SetParent(SystemType);
            return std::move(system);
        }

        static void AddStartEvent(const std::function<void()>& event, int order = 0);
        static void AddStopEvent(const std::function<void()>& event, int order = 0);
        static void AddUpdateEvent(const std::function<void()>& event, int order = 0);
        static std::vector<std::reference_wrapper<System>>& RuntimeSystems();

        static void Start()
        {
            for (auto system : runtimeSystems)
                World::AddSystem(system);

            for (auto& event : startEvents | std::views::values)
                event();

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

        static inline std::vector<std::reference_wrapper<System>> runtimeSystems;
        static inline std::multimap<int, std::function<void()>> startEvents;
        static inline std::multimap<int, std::function<void()>> updateEvents;
        static inline std::multimap<int, std::function<void()>> stopEvents;
        inline static bool isStopping = false;
    };

#define Gleam_Engine_Friend\
    Gleam_MakeType_Friend\
    friend class Engine;

    ///利用如下宏实现关系到程序整个运行周期的事件，如库初始化。
    ///这些事件与System中的Start和Stop不同，System在程序运行中可能多次Start和Stop，
    ///且System::Stop在实体回收前执行，如果在Stop逆初始化库，这可能导致实体中需要该库的数据可能无法正常回收。
#define Gleam_AddEngineEvent(eventType, eventName, order) \
inline void eventName();\
constexpr int eventName##Order = order;\
Gleam_MakeInitEvent(){Engine::Add##eventType##Event(eventName,eventName##Order);}\
inline void eventName()
#define Gleam_AddStartEvent(eventName, order) Gleam_AddEngineEvent(Start, eventName, order)
#define Gleam_AddStopEvent(eventName, order) Gleam_AddEngineEvent(Stop, eventName, order)
#define Gleam_AddUpdateEvent(eventName, order) Gleam_AddEngineEvent(Update, eventName, order)

#define Gleam_Main \
    inline int main()\
    {\
        Gleam::Engine::Start();\
        return 0;\
    }

    ///创建全局作用域的系统单例，变量名与系统类名相同。同时创建一个类别名<systemType>_T，以解决命名冲突。
#define Gleam_MakeGlobalSystem(systemClass,...) \
inline systemClass Global##systemClass = ::Gleam::Engine::CreateSystem<systemClass>(__VA_ARGS__);

    ///将系统添加到世界，并注册到运行时系统组
#define Gleam_AddSystems(...) Gleam_MakeInitEvent(){::Gleam::Engine::RuntimeSystems().insert(::Gleam::Engine::RuntimeSystems().end(),{__VA_ARGS__});}
}