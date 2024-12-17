#pragma once
#include <cassert>
#include <ranges>
#include <set>

namespace Light
{
    class SystemGroup;
    class System
    {
    public:
        constexpr static int32_t LeftOrder = std::numeric_limits<int32_t>::lowest();
        constexpr static int32_t RightOrder = std::numeric_limits<int32_t>::max();
        constexpr static int32_t MiddleOrder = 0;

        SystemGroup* const group;
        const int order;

        System(SystemGroup* group, const int order = MiddleOrder)
            : group(group), order(order)
        {
        }
        System(SystemGroup* group, const int minOrder, const int maxOrder)
            : System(group, static_cast<int32_t>((static_cast<int64_t>(minOrder) + static_cast<int64_t>(maxOrder)) / 2))
        {
            assert(minOrder <= maxOrder && "最大顺序不能小于最小顺序！");
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
        SystemGroup(SystemGroup* group, const int order)
            : System(group, order)
        {
        }
        SystemGroup(SystemGroup* group, const int minOrder, const int maxOrder)
            : System(group, minOrder, maxOrder)
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
}
