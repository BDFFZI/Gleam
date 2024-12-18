/*
接入Light引擎进行游戏开发，在代码层面上通常需要做两件事：

- 启动Light

    将Light静态库链接，编写main函数调用Light::Engine::Launch()即可启动Light。

- 扩展Light

    制作一款游戏需要两种文件：
    1. 资源文件：可通过Editor功能导入和使用。
    2. 代码文件：需要自行扩展Light库来实现。
    所以接下来就请借助Light中的各种模块，实现你的游戏代码...
 */

#include <LightEngine/Runtime/Engine.h>

int main()
{
    Light::Engine::Launch();

    return 0;
}