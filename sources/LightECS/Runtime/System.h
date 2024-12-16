#pragma once
#include <ranges>
#include <set>
#include <typeindex>

// template <class TSystem>
// concept System = requires()
// {
//     TSystem::Order;
//     TSystem::Start;
//     TSystem::Update;
//     TSystem::Stop;
// };

/**
 * 由于System采样静态+模板的方式实现，必须额外采用一个通用数据容器来实现多态
 */
struct SystemInfo
{
    using Function = void(*)();

    std::type_index type = typeid(void);
    int order;
    Function start;
    Function update;
    Function stop;

    bool operator<(const SystemInfo& other) const
    {
        if (order == other.order)
            return type.hash_code() < other.type.hash_code();

        return order < other.order;
    }
};

constexpr int32_t SystemMinOrder = std::numeric_limits<int32_t>::lowest();
constexpr int32_t SystemMaxOrder = std::numeric_limits<int32_t>::max();

class SystemGroup;
class System
{
public:
    const SystemGroup* group;
    const int order;

    System(SystemGroup* group, const int order = 0)
        : group(group), order(order)
    {
    }
    System(SystemGroup* group, const int minOrder, const int maxOrder)
        : System(group, static_cast<int32_t>((static_cast<int64_t>(minOrder) + static_cast<int64_t>(maxOrder)) / 2))
    {
        static_assert(minOrder <= maxOrder, "最大顺序不能小于最小顺序！");
    }
    virtual ~System() = default;
    System(System& system) = delete;

    virtual void OnStart()
    {
    }
    virtual void OnStop()
    {
    }
    virtual void OnUpdate()
    {
    }
};

class SystemGroup : public System
{
public:
    SystemGroup(SystemGroup* group, const int order = 0)
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

    void OnUpdate() override
    {
        OnBeginUpdate();

        if (subSystemStopQueue.empty() == false)
        {
            for (System* system : std::ranges::reverse_view(subSystemStopQueue))
                system->OnStop();
            subSystemStopQueue.clear();
        }

        if (subSystemStartQueue.empty() == false)
        {
            for (System* system : subSystemStartQueue)
                system->OnStart();
            subSystemUpdateQueue.insert(subSystemStartQueue.begin(), subSystemStartQueue.end());
            subSystemStartQueue.clear();
        }

        for (System* system : subSystemUpdateQueue)
        {
            system->OnUpdate();
        }

        OnEndUpdate();
    }

    virtual void OnBeginUpdate()
    {
    }
    virtual void OnEndUpdate()
    {
    }

private:
    std::set<System*> subSystemStartQueue = {};
    std::set<System*> subSystemStopQueue = {};
    std::set<System*> subSystemUpdateQueue = {};
};
