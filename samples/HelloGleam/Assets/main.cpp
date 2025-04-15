#include <GleamEngine/Runtime/Engine.h> //引入 Gleam 的 Engine 功能
#include <iostream>

class MySystem : public Gleam::System<> // 实现System类
{
    void Start() override //自定义Start事件
    {
        std::cout << "Hello Gleam!" << std::endl;
        Gleam::Engine::Stop(); //停止引擎
    }
};
Gleam_MakeRuntimeSystem(MySystem) //将MySystem注册为全局运行时系统，这样在引擎运行时，MySystem将被自动添加到世界

Gleam_Main //运行引擎