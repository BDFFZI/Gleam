addTests()

target_link_libraries("${ModuleName}" PUBLIC LightImport)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})