# 额外的打包功能支持
set(PackPath "" CACHE STRING "打包输出位置")
set(PackTarget "" CACHE STRING "打包目标")
set(PackType "" CACHE STRING "打包类型")

set_target_properties(PROPERTIES RUNTIME_OUTPUT_DIRECTORY)
set(LastOutputPath ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PackPath})
add_custom_target(Pack
    DEPENDS ${PackTarget}
    COMMAND cmake -E copy_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PackType} ${PackPath}
    COMMAND cmake -E copy_directory ${${ProjectName}Resources} ${PackPath}/Resources
    COMMAND echo "打包完成"
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LastOutputPath})