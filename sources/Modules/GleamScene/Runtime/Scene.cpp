#include "Scene.h"

namespace Gleam
{
    Scene& Scene::Create(const std::string_view name)
    {
        Scene scene;
        scene.name = name;
        return allScenes.emplace(scene.name, std::move(scene)).first->second;
    }

    const std::string& Scene::GetName() const
    {
        return name;
    }
    void Scene::AddEntity(Entity entity)
    {
        entities.emplace(entity);
    }
    void Scene::AddSystem(System& system)
    {
        systems.emplace(&system);
    }
}