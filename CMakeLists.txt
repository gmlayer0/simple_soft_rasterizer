cmake_minimum_required(VERSION 3.25)
project(simple_soft_rasterizer)

set(CMAKE_CXX_STANDARD 17)

link_directories(lib/glfd/lib-mingw-w64)
include_directories(lib/glfd/include)

add_executable(simple_soft_rasterizer main.cpp lib/glfd/src/glad.c)
target_link_libraries(simple_soft_rasterizer libglfw3.a)