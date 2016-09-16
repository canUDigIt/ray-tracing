cmake_minimum_required(VERSION 3.5)
project(ray_tracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ../libs/glm
)
set(SOURCE_FILES
    main.cpp
)

add_executable(ray_tracing ${SOURCE_FILES})
