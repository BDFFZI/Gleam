# 额外的打包功能支持
set(PackPath "" CACHE STRING "打包输出位置")
set(PackTarget "" CACHE STRING "打包目标")
set(PackType "" CACHE STRING "打包类型")

if(NOT PackTarget STREQUAL "")
    get_target_property(OUTPUT_DIRECTORY ${PackTarget} RUNTIME_OUTPUT_DIRECTORY)
    string(REPLACE ";" "\" \"" OUTPUT_RESOURCES "\"${${PackTarget}Resources}\"")
    add_custom_target(Pack
        DEPENDS ${PackTarget}
        COMMAND cmake -E copy_directory "${OUTPUT_DIRECTORY}/${PackType}" "${PackPath}"
        COMMAND if \"${${PackTarget}Resources}\" NEQ \"\" cmake -E copy_directory ${OUTPUT_RESOURCES} "${PackPath}/Resources"
        COMMENT "打包成功"
    )
endif()