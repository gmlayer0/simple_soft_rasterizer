cmake_minimum_required(VERSION 3.25)
project(simple_soft_rasterizer)

set(CMAKE_CXX_STANDARD 17)

link_directories(lib/glfd/lib-mingw-w64)
include_directories(lib/glfd/include)

set(SDL2_PATH "./lib/SDL2-2.26.5/x86_64-w64-mingw32")
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
link_directories(./lib/SDL2-2.26.5/x86_64-w64-mingw32/lib)

add_executable(simple_soft_rasterizer main.cpp rasterizer.cpp vertex.h primitive.h utils.h)
target_link_libraries(simple_soft_rasterizer SDL2)