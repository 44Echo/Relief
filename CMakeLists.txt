cmake_minimum_required(VERSION 3.28)
project(Relief LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
add_executable(Relief src/main.cpp)
target_link_libraries(Relief PRIVATE d3d11)