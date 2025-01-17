#pragma once
#include <cassert>
#include <functional>
#include <memory>
#include <ranges>
#include <set>

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

        static inline std::vector<std::unique_ptr<System>> allSystems;
        template <typename TSystem>
        static TSystem* MakeSystem()
        {
            TSystem* system = new TSystem();
            allSystems.emplace_back(system);
            return system;
        }

        SystemGroup* const group;
        const int minOrder;
        const int maxOrder;
        const int order;

        System(SystemGroup* group, const int minOrder = LeftOrder, const int maxOrder = RightOrder)
            : group(group), minOrder(minOrder), maxOrder(maxOrder),
              order(static_cast<int32_t>((static_cast<int64_t>(minOrder) + static_cast<int64_t>(maxOrder)) / 2))
        {
            assert(minOrder <= maxOrder && "最大顺序不能小于最小顺序！");
        }
        System(System* system, const OrderRelation orderRelation)
            : System(system->group,
                     orderRelation == OrderRelation::Before ? system->minOrder : system->order,
                     orderRelation == OrderRelation::Before ? system->order : system->maxOrder)
        {
        }
        virtual ~System() = default;
        System(System& system) = delete;

        virtual void Start()
        {
        }
        virtual void Stop()
        {
        }
        virtual void Update()
        {
        }
    };

    class SystemGroup : public System
    {
    public:
        SystemGroup(SystemGroup* group, const int minOrder = LeftOrder, const int maxOrder = RightOrder)
            : System(group, minOrder, maxOrder)
        {
        }
        SystemGroup(System* system, const OrderRelation orderRelation)
            : System(system, orderRelation)
        {
        }

        void AddSubSystem(System& system)
        {
            subSystemStartQueue.insert(&system);
        }
        void RemoveSubSystem(System& system)
        {
            subSystemUpdateQueue.erase(&system);
            subSystemStopQueue.insert(&system);
        }

        void Start() override;
        void Stop() override;
        void Update() override;

    private:
        friend class EditorUIUtility;

        struct SystemPtrComparer
        {
            bool operator()(const System* left, const System* right) const
            {
                if (left->order == right->order)
                    return left < right; //确保顺序相同时依然有大小之分，从而避免被误认为相等
                return left->order < right->order;
            }
        };

        std::set<System*, SystemPtrComparer> subSystemStartQueue = {};
        std::set<System*, SystemPtrComparer> subSystemStopQueue = {};
        std::set<System*, SystemPtrComparer> subSystemUpdateQueue = {};
    };

    class SystemEvent : public System
    {
    public:
        std::function<void()> onStart = nullptr;
        std::function<void()> onStop = nullptr;
        std::function<void()> onUpdate = nullptr;

        SystemEvent(SystemGroup* group, const int minOrder = LeftOrder, const int maxOrder = RightOrder)
            : System(group, minOrder, maxOrder)
        {
        }
        SystemEvent(System* system, const OrderRelation orderRelation)
            : System(system, orderRelation)
        {
        }

    private:
        void Start() override { if (onStart) onStart(); }
        void Stop() override { if (onStop) onStop(); }
        void Update() override { if (onUpdate) onUpdate(); }
    };

#define Light_MakeSystem(systemClass) \
    inline systemClass* systemClass = Light::System::MakeSystem<class systemClass##>();
}