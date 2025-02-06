#pragma once

#include "LightECS/Runtime/Entity.h"
#include "LightECS/Runtime/Scene.h"
#include "LightECS/Runtime/System.h"

namespace Light
{
    class EditorUIUtility
    {
    public:
        static void DrawEntityButton(Entity entity);
        static void DrawEntity(Entity entity);
        static void DrawSystemGroup(SystemGroup* systemGroup);
        static void DrawSystemGroupUnfolding(SystemGroup* systemGroup);
        static void DrawScene(Scene* scene);
        static void DrawSceneUnfolding(Scene* scene);
  };
}