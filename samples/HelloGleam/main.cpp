#include <GleamEngine/Runtime/Engine.h> //引入 Gleam 的 Engine 功能
#include "GleamECS/Runtime/World.h"// 引入World功能
#include <iostream>

class MySystem : public Gleam::System// 实现System类
{
    void Start() override//自定义Start事件
    {
        std::cout << "Hello Gleam!" << std::endl;
    }
};
Gleam_MakeGlobalSystem(MySystem)//创建System实例
Gleam_AddSystems(MySystem)//将System实例添加到World中

Gleam_Main//运行引擎