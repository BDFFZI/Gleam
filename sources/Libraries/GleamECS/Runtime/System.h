#pragma once
#include <functional>
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

        static auto GetAllSystems()
        {
            return allSystems | std::views::values | std::views::transform(
                [](auto system) { return std::reference_wrapper<System>(*system); }
            );
        }

        /**
         * 全局System创建函数。
         *
         * 全局系统将添加到全局统计容器中，这些被统计的系统将可以被Scene实现序列化。
         * 注意！全局系统默认用TypeID做索引，故每种类型的System默认只能有一个全局系统
         * 
         * @tparam TSystem 
         * @return 
         */
        template <typename TSystem> requires std::derived_from<TSystem, System>
        static TSystem CreateGlobal(std::string_view name = "", uuids::uuid id = {})
        {
            Type& systemType = Type::CreateOrGet<TSystem>();
            TSystem system;
            //设置名称
            if (!name.empty())
                system.name = name;
            else if (system.name.empty())
            {
                //生成默认名称
                std::string defaultName = std::string(typeid(TSystem).name());
                defaultName = defaultName.substr(defaultName.find_last_of(' ') + 1);
                system.name = defaultName;
            }
            //设置编号
            if (!id.is_nil())
                system.id = id;
            else if (system.id.is_nil())
            {
                //生成默认ID
                system.id = systemType.GetID();
            }
            //设置父类
            if (!systemType.GetParent().has_value()) //生成默认父类
                systemType.SetParent(Type::GetType(typeid(System)).value());
            //注册索引
            assert(!allSystems.contains(system.id) && "已有相同ID的系统已被注册！");
            allSystems.emplace(system.id, &system);

            return system;
        }
        /**
         * 一个更完善的System创建函数。
         *
         * 相比自行构造System，该函数能显式设置系统名称以及注册类型信息。
         * @tparam TSystem 
         * @param name 
         * @return 
         */
        template <typename TSystem> requires std::derived_from<TSystem, System>
        static TSystem Create(const std::string_view name)
        {
            Type& systemType = Type::CreateOrGet<TSystem>();
            TSystem system;
            //设置名称
            system.name = name;
            //设置ID（非全局系统，ID无效）
            system.id = "";
            //设置Type的父类信息
            if (!systemType.GetParent().has_value())
                systemType.SetParent(Type::GetType(typeid(System)).value());

            return system;
        }
        static std::optional<std::reference_wrapper<System>> GetSystem(uuids::uuid id);

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

        inline static std::unordered_map<uuids::uuid, System*> allSystems = {};

        std::string name;
        uuids::uuid id;
        std::optional<std::reference_wrapper<SystemGroup>> group;
        int minOrder;
        int maxOrder;
        int order;
    };

    Gleam_MakeType(System, "")
    {
        Gleam_MakeType_AddField(minOrder);
        Gleam_MakeType_AddField(maxOrder);
        Gleam_MakeType_AddField(order);
        Gleam_MakeType_AddField(group);
        Gleam_MakeType_AddField(name);
    }

    class SystemEvent : public System
    {
    public:
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
                return left < right; //确保顺序相同时依然有大小之分，从而避免被误认为相等
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

        void Start() override;
        void Stop() override;
        void Update() override
        {
            if (subSystemStopQueue.empty() == false)
            {
                for (System* system : std::ranges::reverse_view(subSystemStopQueue))
                    system->Stop();
                subSystemStopQueue.clear();
            }

            if (subSystemStartQueue.empty() == false)
            {
                for (System* system : subSystemStartQueue)
                    system->Start();
                subSystemUpdateQueue.insert(subSystemStartQueue.begin(), subSystemStartQueue.end());
                subSystemStartQueue.clear();
            }

            for (System* system : subSystemUpdateQueue)
            {
#ifdef GleamEngineEditor
                auto& name = system->GetName();
                Gleam_ProfilerSample(name);
#endif
                system->Update();
            }
        }

    private:
        friend class HierarchyWindow;

        std::set<System*, SystemPtrComparer> subSystemStartQueue = {};
        std::set<System*, SystemPtrComparer> subSystemStopQueue = {};
        std::set<System*, SystemPtrComparer> subSystemUpdateQueue = {};
    };

#define Gleam_MakeGlobalSystem(systemClass,...) \
inline systemClass Global##systemClass = ::Gleam::System::CreateGlobal<systemClass>(__VA_ARGS__);
}