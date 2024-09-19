addTests()

find_package(glm CONFIG REQUIRED)
target_link_libraries(${ModuleName} PRIVATE glm::glm)

find_package(Stb REQUIRED)
target_include_directories(${ModuleName} PRIVATE ${Stb_INCLUDE_DIR})

find_package(tinyobjloader CONFIG REQUIRED)
target_link_libraries(${ModuleName} PRIVATE tinyobjloader::tinyobjloader)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})