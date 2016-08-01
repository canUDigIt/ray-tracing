cmake_minimum_required(VERSION 3.5)
project(ray_tracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp vec3.cpp vec3.h ray.h hitable.h hitable_list.h hitable_list.cpp sphere.h sphere.cpp)
add_executable(ray_tracing ${SOURCE_FILES})