/*
接入Gleam引擎进行游戏开发，在代码层面上通常需要做两件事：

- 启动Gleam

    将Gleam静态库链接，编写main函数调用Gleam::Engine::Launch()即可启动Gleam。

- 扩展Gleam

    制作一款游戏需要两种文件：
    1. 资源文件：可通过Editor功能导入和使用。
    2. 代码文件：需要自行扩展Gleam库来实现。
    所以接下来就请借助Gleam中的各种模块，实现你的游戏代码...
 */

#include <iostream>
#include <ostream>
#include <GleamEngine/Runtime/Engine.h>

class MySystem : public Gleam::System
{
    void Start() override
    {
        std::cout << "Hello Gleam!" << std::endl;
    }
};
Gleam_MakeSystemInstance(MySystem)
Gleam_AddSystems(MySystem)