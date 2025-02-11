#pragma once
#include <map>
#include <memory>
#include <vector>

#include "GleamECS/Runtime/System.h"

namespace Gleam
{
    class Engine
    {
    public:
        static void AddStartEvent(const std::function<void()>& event, int order = 0);
        static void AddStopEvent(const std::function<void()>& event, int order = 0);
        static void AddUpdateEvent(const std::function<void()>& event, int order = 0);
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

    ///创建系统并非一定要调用该函数。（我甚至在考虑移除该函数，改为让用户自行创建单例）
    ///系统本质只是一种接收事件的接口，唯一的要求就是继承System。所以系统就像实体一样是可以由用户自由创建增删。
    ///该宏用于创建全局作用域的系统单例，变量名与系统类名相同，因此后续再次使用系统类时可能存在问题，
    ///故该宏还会利用using创建一个系统类型别名：<systemType>_T
#define Gleam_MakeSystemInstance(systemClass) \
using systemClass##_T = systemClass;\
inline systemClass* systemClass = Gleam::Engine::RegisterGlobalSystem<class systemClass##>();

    ///将系统添加到世界，并注册到运行时系统组
#include "GleamUtility/Runtime/Program.h"
#define Gleam_AddSystems(...) Gleam_MakeInitEvent(){::Gleam::Engine::RuntimeSystems().insert(::Gleam::Engine::RuntimeSystems().end(),{__VA_ARGS__});}
}