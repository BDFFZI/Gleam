#include "System.h"

namespace Light
{
    void SystemGroup::Start()
    {
        if (subSystemStartQueue.empty() == false)
        {
            for (System* system : subSystemStartQueue)
                system->Start();
            subSystemUpdateQueue.insert(subSystemStartQueue.begin(), subSystemStartQueue.end());
            subSystemStartQueue.clear();
        }
    }
    void SystemGroup::Stop()
    {
        if (subSystemStopQueue.empty() == false)
        {
            for (System* system : std::ranges::reverse_view(subSystemStopQueue))
                system->Stop();
            subSystemStopQueue.clear();
        }

        if (subSystemUpdateQueue.empty() == false)
        {
            for (System* system : std::ranges::reverse_view(subSystemUpdateQueue))
                system->Stop();
            subSystemUpdateQueue.clear();
        }

        subSystemStartQueue.clear();
    }
    void SystemGroup::Update()
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
            system->Update();
    }
}
