#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <set>
#include <string>

namespace Light
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
        constexpr static int32_t LeftOrder = std::numeric_limits<int32_t>::lowest();
        constexpr static int32_t RightOrder = std::numeric_limits<int32_t>::max();
        constexpr static int32_t MiddleOrder = 0;

        /**
         * minOrder和maxOrder不仅用于计算System的执行顺序，同时暗示了System的作用域，例如可以借此有意限制必须在其他系统之前执行。
         * @param group 
         * @param minOrder 
         * @param maxOrder 
         */
        explicit System(std::optional<SystemGroup*> group = std::nullopt, int minOrder = LeftOrder, int maxOrder = RightOrder);
        /**
         * 根据与其他System的相对顺序计算当前System顺序，计算时会注意遵从被依赖System的作用域。
         * @param system 
         * @param orderRelation 
         */
        System(System* system, OrderRelation orderRelation);
        virtual ~System() = default;

        std::string& GetName();
        std::optional<SystemGroup*> GetGroup() const;
        int GetOrder() const;
        
        virtual void Start();
        virtual void Update();
        virtual void Stop();

    private:
        friend class SystemEvent;

        std::string name;
        std::optional<SystemGroup*> group;
        int minOrder;
        int maxOrder;
        int order;
    };

    class SystemEvent : public System
    {
    public:
        std::function<void()> onStart = nullptr;
        std::function<void()> onStop = nullptr;
        std::function<void()> onUpdate = nullptr;

        SystemEvent(std::string_view name, std::optional<SystemGroup*> group, int minOrder = LeftOrder, int maxOrder = RightOrder);
        SystemEvent(std::string_view name, System* system, OrderRelation orderRelation);

    private:
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
        SystemGroup(std::optional<SystemGroup*> group, int minOrder = LeftOrder, int maxOrder = RightOrder);
        SystemGroup(System* system, OrderRelation orderRelation);

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