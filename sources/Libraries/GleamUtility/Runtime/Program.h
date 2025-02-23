#pragma once

#define Gleam_MakeInitEvent_Inner2(id,id2) \
void InitEvent##id##id2();\
inline int InitEvent##id##id2##Executor = (InitEvent##id##id2(),0);\
inline void InitEvent##id##id2()
#define Gleam_MakeInitEvent_Inner(id,id2) Gleam_MakeInitEvent_Inner2(id,id2)

//如果无法正常触发初始化，尝试在全局空间使用，命名空间中的全局变量可能不被初始化
#define Gleam_MakeInitEvent() Gleam_MakeInitEvent_Inner(__COUNTER__,__LINE__)