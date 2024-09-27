addModule()

target_link_libraries("${ModuleName}" PUBLIC LightMath)

find_package(Stb REQUIRED)
target_include_directories("${ModuleName}" PUBLIC ${Stb_INCLUDE_DIR})

find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries("${ModuleName}" PUBLIC tinyobjloader::tinyobjloader)