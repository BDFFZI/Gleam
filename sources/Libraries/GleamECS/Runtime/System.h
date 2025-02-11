#pragma once
#include <functional>
#include <optional>
#include <set>
#include <string>


namespace Gleam
{
    enum class OrderRelation:uint8_t
    {
        Before,
        After
    };

    class SystemGroup;
    class System
    {
    public:
        explicit System(std::string_view name, std::optional<std::reference_wrapper<SystemGroup>> group = std::nullopt);
        /**
         * 根据与其他System的相对顺序计算当前System顺序，计算时会注意遵从被依赖System的作用域。
         * @param name
         * @param system 
         * @param orderRelation 
         */
        System(std::string_view name, System& system, OrderRelation orderRelation);
        virtual ~System() = default;

        std::string& GetName();
        std::optional<std::reference_wrapper<SystemGroup>> GetGroup() const;
        int GetOrder() const;

        virtual void Start();
        virtual void Update();
        virtual void Stop();

    private:
        friend class SystemEvent;

        std::string name;
        std::optional<std::reference_wrapper<SystemGroup>> group;
        int minOrder;
        int maxOrder;
        int order;

        System(std::string_view name, std::optional<std::reference_wrapper<SystemGroup>> group, int minOrder, int maxOrder);
    };

    class SystemEvent : public System
    {
    public:
        std::function<void()>& OnStart();
        std::function<void()>& OnStop();
        std::function<void()>& OnUpdate();

        SystemEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group)
            : System(name, group)
        {
        }
        SystemEvent(const std::string_view& name, System& system, const OrderRelation orderRelation)
            : System(name, system, orderRelation)
        {
        }

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
        SystemGroup(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>> group = std::nullopt)
            : System(name, group)
        {
        }
        SystemGroup(const std::string_view& name, System& system, const OrderRelation orderRelation)
            : System(name, system, orderRelation)
        {
        }

        /**
         * 统计所有子系统，包括递归，但不包含正在卸载的子系统。
         * 也即是在用户调用AddSubSystem和RemoveSubSystem后逻辑上子系统集合。
         * 排除生命周期的影响，用该结果反向插入后可重建相同的系统组。
         * @return 
         */
        std::vector<System*> GetSubSystems();
        void AddSubSystem(System* system);
        void RemoveSubSystem(System* system);

        void Start() override;
        void Stop() override;
        void Update() override;

    private:
        friend class EditorUI;

        std::set<System*, SystemPtrComparer> subSystemStartQueue = {};
        std::set<System*, SystemPtrComparer> subSystemStopQueue = {};
        std::set<System*, SystemPtrComparer> subSystemUpdateQueue = {};
    };
}