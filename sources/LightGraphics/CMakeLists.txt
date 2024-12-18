addModule()

target_link_libraries("${ModuleName}" PRIVATE LightGL)

target_link_libraries("${ModuleName}" PRIVATE LightImport)

target_link_libraries("${ModuleName}" PRIVATE LightUtility)
