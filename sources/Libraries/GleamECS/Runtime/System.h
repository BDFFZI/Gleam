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
        explicit System(std::optional<std::reference_wrapper<SystemGroup>> group, std::string_view name = "");
        System(System& system, OrderRelation orderRelation, std::string_view name = "");
        explicit System(SystemGroup& group);
        System(System&) = delete;
        System& operator=(System&) = delete;
        System(System&&) = default;
        System& operator=(System&&) = default;
        virtual ~System() = default;

        std::string& GetName();
        std::optional<std::reference_wrapper<SystemGroup>> GetGroup() const;
        int GetOrder() const;

        virtual void Start();
        virtual void Update();
        virtual void Stop();

    protected:
        System(
            std::string_view name = "",
            std::optional<std::reference_wrapper<SystemGroup>> group = std::nullopt,
            int minOrder = std::numeric_limits<int32_t>::lowest(),
            int maxOrder = std::numeric_limits<int32_t>::max()
        );

    private:
        friend class SystemEvent;

        std::string name;
        std::optional<std::reference_wrapper<SystemGroup>> group;
        int minOrder;
        int maxOrder;
        int order;
    };

    class SystemEvent : public System
    {
    public:
        SystemEvent(const std::string_view& name, const std::optional<std::reference_wrapper<SystemGroup>>& group)
            : System(group, name)
        {
        }
        SystemEvent(const std::string_view& name, System& system, const OrderRelation orderRelation)
            : System(system, orderRelation, name)
        {
        }
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
        SystemGroup(const std::optional<std::reference_wrapper<SystemGroup>>& group, const std::string_view& name = "")
            : System(group, name)
        {
        }
        SystemGroup(System& system, const OrderRelation orderRelation, const std::string_view& name = "")
            : System(system, orderRelation, name)
        {
        }
        explicit SystemGroup(SystemGroup& group)
            : System(group)
        {
        }
        SystemGroup(SystemGroup&&) noexcept = default;
        SystemGroup& operator=(SystemGroup&&) = default;

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