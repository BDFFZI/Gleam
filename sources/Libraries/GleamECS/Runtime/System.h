#pragma once
#include <functional>
#include <iostream>
#include <optional>
#include <ranges>
#include <set>
#include <string>

#include "GleamReflection/Runtime/Type.h"

#ifdef GleamEngineEditor
#include "GleamEngine/Editor/Profiler.h"
#endif

namespace Gleam
{
    enum class OrderRelation:uint8_t
    {
        Before,
        After
    };

    class SystemGroup;

    /**
     * 系统是一种高级的封装版事件。
     * 通过相对位置、嵌套等结构，实现在引擎中自由轻松的嵌入各种代码。
     */
    class System
    {
    public:
        static constexpr int32_t MinOrder = std::numeric_limits<int32_t>::lowest();
        static constexpr int32_t MaxOrder = std::numeric_limits<int32_t>::max();
        static constexpr int32_t DefaultOrder = 0;

        static auto GetAllGlobalSystems()
        {
            return allGlobalSystems | std::views::values | std::views::transform(
                [](auto& system) { return std::reference_wrapper<System>(*system); }
            );
        }

        template <typename TSystem> requires std::derived_from<TSystem, System>
        static TSystem& CreateGlobal(std::string_view name = "", const uuids::uuid id = {})
        {
            std::unique_ptr<System> system = std::unique_ptr<System>{new TSystem()};
            //设置父类
            Type& systemType = Type::CreateOrGet<TSystem>();
            if (!systemType.GetParent().has_value()) //生成默认父类
                systemType.SetParent(Type::GetType(typeid(System)).value());
            //设置名称
            if (!name.empty())
                system->name = name;
            else if (system->name.empty())
            {
                //生成默认名称
                std::string defaultName = std::string(typeid(TSystem).name());
                defaultName = defaultName.substr(defaultName.find_last_of(' ') + 1);
                system->name = defaultName;
            }
            //设置编号
            if (!id.is_nil())
                system->id = id;
            else if (system->id.is_nil())
                system->id = MD5(name.data()).toArray();
            //注册索引
            assert(!allGlobalSystems.contains(system->id) && "已有相同ID的系统已被注册！");
            auto& result = allGlobalSystems.emplace(system->id, std::move(system)).first->second;

            return *static_cast<TSystem*>(result.get());
        }
        template <typename TSystem> requires std::derived_from<TSystem, System>
        static TSystem Create(std::string_view name)
        {
            TSystem system;

            //设置父类
            Type& systemType = Type::CreateOrGet<TSystem>();
            if (!systemType.GetParent().has_value()) //生成默认父类
                systemType.SetParent(Type::GetType(typeid(System)).value());
            //设置名称
            system.name = name;

            return system;
        }

        static std::optional<std::reference_wrapper<System>> GetGlobalSystem(uuids::uuid id);

        System();
        explicit System(std::optional<std::reference_wrapper<SystemGroup>> group, int minOrder = MinOrder, int maxOrder = MaxOrder, std::string_view name = "");
        System(System& system, OrderRelation orderRelation, std::string_view name = "");
        explicit System(SystemGroup& group);
        System(System&) = delete;
        System& operator=(System&) = delete;
        System(System&&) = default;
        System& operator=(System&&) = default;
        virtual ~System() = default;

        const std::string& GetName() const;
        uuids::uuid GetID() const;
        std::optional<std::reference_wrapper<SystemGroup>> GetGroup() const;
        int GetOrder() const;

        virtual void Start();
        virtual void Update();
        virtual void Stop();

    private:
        friend class SystemEvent;
        Gleam_MakeType_Friend

        inline static std::unordered_map<uuids::uuid, std::unique_ptr<System>> allGlobalSystems = {};

        std::string name;
        uuids::uuid id;
        std::optional<std::reference_wrapper<SystemGroup>> group;
        int minOrder;
        int maxOrder;
        int order;
    };

    Gleam_MakeType(System, "")
    {
        Gleam_MakeType_AddField(name);
        Gleam_MakeType_AddField(id);
        Gleam_MakeType_AddField(group);
        Gleam_MakeType_AddField(minOrder);
        Gleam_MakeType_AddField(maxOrder);
        Gleam_MakeType_AddField(order);
    }

    class SystemEvent : public System
    {
    public:
        static SystemEvent StartEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int order, std::function<void()> startEvent);
        static SystemEvent UpdateEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int order, std::function<void()> updateEvent);
        static SystemEvent StopEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int order, std::function<void()> stopEvent);

        SystemEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group, int minOrder = MinOrder, int maxOrder = MaxOrder);
        SystemEvent(const std::string_view& name, System& system, OrderRelation orderRelation);
        SystemEvent(SystemEvent&&) noexcept = default;
        SystemEvent& operator=(SystemEvent&&) = default;

        std::function<void()>& OnStart();
        std::function<void()>& OnStop();
        std::function<void()>& OnUpdate();

    private:
        std::function<void()> onStart = nullptr;
        std::function<void()> onStop = nullptr;
        std::function<void()> onUpdate = nullptr;

        void Start() override;
        void Stop() override;
        void Update() override;
    };

    struct SystemPtrComparer
    {
        bool operator()(const System* left, const System* right) const
        {
            if (left->GetOrder() == right->GetOrder())
                return left < right; //确保顺序相同时依然有大小之分，从而避免不同系统实例被误认为相等
            return left->GetOrder() < right->GetOrder();
        }
    };

    class SystemGroup : public System
    {
    public:
        SystemGroup(const std::optional<std::reference_wrapper<SystemGroup>>& group, int minOrder = MinOrder, int maxOrder = MaxOrder, const std::string_view& name = "");
        SystemGroup(System& system, OrderRelation orderRelation, const std::string_view& name = "");
        explicit SystemGroup(SystemGroup& group);
        SystemGroup(SystemGroup&&) noexcept = default;
        SystemGroup& operator=(SystemGroup&&) = default;

        /**
         * 统计所有子系统，包括递归，但不包含正在卸载的子系统。
         * 也即是在用户调用AddSubSystem和RemoveSubSystem后逻辑上子系统集合。
         * 排除生命周期的影响，用该结果反向插入后可重建相同的系统组。
         * @return 
         */
        std::vector<std::reference_wrapper<System>> GetSubSystems();
        void AddSubSystem(System& system);
        void RemoveSubSystem(System& system);

        void Update() override
        {
            for (System* system : subSystems)
            {
#ifdef GleamEngineEditor
                auto& name = system->GetName();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
        }

        void FlushStartQueue();
        void FlushStopQueue();
        void Clear();

    private:
        friend class HierarchyWindow;

        std::set<System*, SystemPtrComparer> subSystems = {};
        /// 虽然子系统的添加删除是延迟的，但并不支持遍历时的结构化更改。
        /// 例如Start时addingSystems被占用，但用户依然可能执行AddSubSystem函数，于是就会导致遍历异常。
        /// 此处延迟触发的真实原因是因为插入系统和删除系统是无序的，但系统本身是有序的，
        /// 为了满足系统的顺序安排，只有先缓存再汇总后才可知正确的执行顺序。
        std::set<System*> addingSystems = {};
        std::set<System*> removingSystems = {};
    };

#define Gleam_MakeGlobalSystem(systemClass) \
inline systemClass& Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>("",::Gleam::Type::CreateOrGet<systemClass>().GetID());

#define Gleam_MakeSystemEvent(name,group,order,type,action) \
inline SystemEvent name = SystemEvent::##type##Event(#name,group,order,action);
}