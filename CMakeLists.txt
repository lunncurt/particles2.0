cmake_minimum_required(VERSION 3.15)
project(particles)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(SOURCE_FILES src/main.cpp)

add_executable(exec ${SOURCE_FILES})

find_package(SFML 2.6.1 COMPONENTS system window graphics REQUIRED)
include_directories(${SFML_INCLUDE_DIRS})
target_link_libraries(exec sfml-system sfml-window sfml-graphics)

find_package(OpenCL REQUIRED)
target_link_libraries(exec OpenCL::OpenCL)
