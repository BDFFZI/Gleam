#pragma once
#include <fstream>
#include <string>

class Utility
{
public:
    static std::string ReadFile(const std::string& filename)
    {
        //通过ate标志初始就将读取位置设在流末尾
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("文件打开失败！");

        //由于读取位置在流末尾，故其位置即文件长度
        const std::streamsize fileSize = file.tellg();
        std::string content(fileSize, '0');

        //读取内容
        file.seekg(0);
        file.read(content.data(), fileSize);

        file.close();

        return content;
    }

#define Light_MakeInitEvent_Inner2(id) \
    void InitEvent##id();\
    inline int InitEvent##id##Executor = (InitEvent##id(),0);\
    inline void InitEvent##id()
#define Light_MakeInitEvent_Inner(id) Light_MakeInitEvent_Inner2(id)
#define Light_MakeInitEvent() Light_MakeInitEvent_Inner(__COUNTER__)
};