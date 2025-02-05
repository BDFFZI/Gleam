#pragma once

#define Light_MakeInitEvent_Inner2(id) \
void InitEvent##id();\
inline int InitEvent##id##Executor = (InitEvent##id(),0);\
inline void InitEvent##id()
#define Light_MakeInitEvent_Inner(id) Light_MakeInitEvent_Inner2(id)

//如果无法正常触发初始化，尝试在全局空间使用，命名空间中的全局变量可能不被初始化
#define Light_MakeInitEvent() Light_MakeInitEvent_Inner(__COUNTER__)