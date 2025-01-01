addProject()

find_package(stduuid CONFIG REQUIRED)
target_link_libraries(${ProjectName} PRIVATE stduuid)

find_package(rapidjson CONFIG REQUIRED)
target_link_libraries(${ProjectName} PRIVATE rapidjson)