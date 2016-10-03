cmake_minimum_required(VERSION 3.5)
project(ray_tracing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(
    ./dependencies/eigen
    ./dependencies/tinyobjloader
)
set(SOURCE_FILES
    main.cpp
)

add_executable(ray_tracing ${SOURCE_FILES})

add_custom_command(TARGET ray_tracing POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:ray_tracing>)
