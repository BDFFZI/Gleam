project(HelloLight)

add_executable(HelloLight main.cpp)

target_link_libraries(HelloLight PRIVATE LightEngine)