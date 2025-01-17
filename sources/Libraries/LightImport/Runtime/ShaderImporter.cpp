#include "ShaderImporter.h"

#include <map>

#include "LightUtility/Runtime/Utility.hpp"

#include <stdexcept>
#include <filesystem>


using namespace Light;

class ShaderIncluder : public shaderc::CompileOptions::IncluderInterface
{
    shaderc_include_result* GetInclude(const char* requested_source,
                                       shaderc_include_type type,
                                       const char* requesting_source,
                                       const size_t include_depth) override
    {
        if (include_depth > 20)
            throw std::runtime_error("包含文件深度过大，请检查是否循环导入。");

        std::string includedString;
        if (std::filesystem::exists(requested_source)) //程序相对位置
            includedString = Utility::ReadFile(requested_source);
        else //文件相对位置
        {
            std::filesystem::path requestingPath(requesting_source);
            std::filesystem::path requestedPath = requestingPath.parent_path() / requested_source;
            includedString = Utility::ReadFile(requestedPath.string());
        }

        char* includedC_String = new char[includedString.length()];
        memcpy(includedC_String, includedString.c_str(), includedString.length());

        shaderc_include_result* includeResult = new shaderc_include_result();
        includeResult->source_name = requested_source;
        includeResult->source_name_length = strlen(requested_source);
        includeResult->content = includedC_String;
        includeResult->content_length = includedString.length();

        return includeResult;
    }

    void ReleaseInclude(shaderc_include_result* data) override
    {
        delete[] data->content;
        delete data;
    }
};

std::vector<std::byte> ShaderImporter::ImportHlsl(const std::string& code, const shaderc_shader_kind type, const std::string& entryPoint)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetSourceLanguage(shaderc_source_language_hlsl);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);

    auto compilationResult = compiler.CompileGlslToSpv(code, type, "[shader]", entryPoint.c_str(), options);
    if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::string errorMessage = compilationResult.GetErrorMessage();
        throw std::runtime_error("编译着色器失败！\n" + errorMessage);
    }

    std::vector bytecode(
        reinterpret_cast<const std::byte*>(compilationResult.begin()),
        reinterpret_cast<const std::byte*>(compilationResult.end())
    );
    return bytecode;
}
std::vector<std::byte> ShaderImporter::ImportHlslFromFile(const std::string& file, const shaderc_shader_kind type, const std::string& entryPoint)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    options.SetSourceLanguage(shaderc_source_language_hlsl);
    options.SetOptimizationLevel(shaderc_optimization_level_performance);
    options.SetIncluder(std::make_unique<ShaderIncluder>());

    //bom处理
    std::string fileCode = Utility::ReadFile(file);
    char bom[] = {static_cast<char>(0xEF), static_cast<char>(0xBB), static_cast<char>(0xBF)};
    if (std::equal(std::begin(bom), std::end(bom), fileCode.data()))
        fileCode = fileCode.substr(3);

    auto preprocessResult = compiler.PreprocessGlsl(fileCode, type, file.c_str(), options);
    if (preprocessResult.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::string errorMessage = preprocessResult.GetErrorMessage();
        throw std::runtime_error("预处理着色器失败！\n" + errorMessage);
    }

    std::string code = preprocessResult.begin();
    return ImportHlsl(code, type, entryPoint);
}