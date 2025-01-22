#pragma once

#define Light_MakeInitEvent_Inner2(id) \
void InitEvent##id();\
inline int InitEvent##id##Executor = (InitEvent##id(),0);\
inline void InitEvent##id()
#define Light_MakeInitEvent_Inner(id) Light_MakeInitEvent_Inner2(id)
#define Light_MakeInitEvent() Light_MakeInitEvent_Inner(__COUNTER__)