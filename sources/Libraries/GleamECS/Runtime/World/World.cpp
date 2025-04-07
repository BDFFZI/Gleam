#include "World.h"

namespace Gleam
{
    Entity World::AddEntity(const Archetype& archetype, const bool addToScene)
    {
        Entity entity = entityAllocator.AddEntity(archetype);

        if (addToScene && activeScene != nullptr)
            activeScene->AddEntity(entity);

        return entity;
    }
    void World::RemoveEntity(Entity& entity, const bool removeFromScene)
    {
        if (removeFromScene)
        {
            auto optionalScene = GetScene(entity);
            if (optionalScene.has_value())
                optionalScene->get().RemoveEntity(entity);
        }

        entityAllocator.RemoveEntity(entity);
    }
    System& World::AddSystem(SystemInfo& systemInfo, const bool addToScene)
    {
        if (systemInfo.group != nullptr)
            AddSystem(*systemInfo.group);

        auto& [system,count] = systems[systemInfo.type];
        ++count;

        if (count == 1) //首次添加
        {
            SystemGroup* group = systemInfo.group == nullptr ? &rootSystem : dynamic_cast<SystemGroup*>(std::get<0>(systems[systemInfo.group->type]).get());
            //创建实例
            system = std::shared_ptr<System>(static_cast<System*>(systemInfo.type->Create()));
            system->world = this;
            system->group = group;
            system->order = systemInfo.order;
            //注册到组
            group->AddSubSystem(*system);
        }

        return *system;
    }
    void World::RemoveSystem(SystemInfo& systemInfo, bool removeFromScene)
    {
        if (systemInfo.group != nullptr)
            RemoveSystem(*systemInfo.group);

        auto& [system,count] = systems.at(systemInfo.type);
        --count;

        if (count == 0) //最终移除
        {
            //从组移除
            SystemGroup* group = systemInfo.group == nullptr ? &rootSystem : dynamic_cast<SystemGroup*>(std::get<0>(systems[systemInfo.group->type]).get());
            group->RemoveSubSystem(*system);
            //销毁实例
            //在Update时执行，因为要等待系统Stop事件执行完毕。
        }
    }
    Scene& World::AddScene(const std::string_view name, const bool isRunning)
    {
        assert(GetScene(name) == std::nullopt && "同名场景已存在！");
        std::unique_ptr<Scene>& scene = allScenes.emplace_back(std::make_unique<Scene>(
            *this, name, isRunning
        ));
        return *scene;
    }
    void World::RemoveScene(Scene& scene, const bool release)
    {
        if (release)
            scene.Release();
        erase_if(allScenes, [&scene](auto& scenePtr) { return scenePtr.get() == &scene; });
    }

    void World::Update()
    {
        rootSystem.Update();
        FlushSystemQueue();
        FlushEntityQueue();
    }
    void World::Clear()
    {
        for (auto& count : systemUsageCount | std::views::values)
            count++; //抑制用户回收方法，防止重复回收
        rootSystem.Stop();
        systemUsageCount.clear();

        entityAllocator.Clear();
        removingEntities.clear();
        entityInfoAllocator.Clear();
    }

    void World::FlushEntityQueue()
    {
        for (const auto& [entity,archetype] : movingEntities)
            entityAllocator.MoveEntity(entity, *archetype);
        movingEntities.clear();

        for (auto [entity] : removingEntities)
            entityAllocator.RemoveEntity(entity);
        removingEntities.clear();
    }
    void World::FlushSystemQueue()
    {
        for (auto it = systems.begin(); it != systems.end();)
        {
            int count = std::get<1>(it->second);
            if (count == 0)
                it = systems.erase(it);
            else
                ++it;
        }
    }
}