#pragma once
#include <functional>
#include <map>

#include "GleamECS/Runtime/System.h"


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
        static TSystem CreateSystem(const std::string_view name = typeid(TSystem).name())
        {
            TSystem system = {};
            if (system.GetName().empty())
                system.GetName() = name;
            return std::move(system);
        }

        static void AddStartEvent(const std::function<void()>& event, int order = 0);
        static void AddStopEvent(const std::function<void()>& event, int order = 0);
        static void AddUpdateEvent(const std::function<void()>& event, int order = 0);
        static std::vector<std::reference_wrapper<System>>& RuntimeSystems();

        static void Start();
        static void Stop();

    private:
        friend class Editor;

        static inline std::vector<std::reference_wrapper<System>> runtimeSystems;
        static inline std::multimap<int, std::function<void()>> startEvents;
        static inline std::multimap<int, std::function<void()>> updateEvents;
        static inline std::multimap<int, std::function<void()>> stopEvents;
        inline static bool isStopping = false;
    };

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
using systemClass##_T = systemClass;\
inline systemClass systemClass = Gleam::Engine::CreateSystem<systemClass##_T>(__VA_ARGS__);

    ///将系统添加到世界，并注册到运行时系统组
#include "GleamUtility/Runtime/Program.h"
#define Gleam_AddSystems(...) Gleam_MakeInitEvent(){::Gleam::Engine::RuntimeSystems().insert(::Gleam::Engine::RuntimeSystems().end(),{__VA_ARGS__});}
}