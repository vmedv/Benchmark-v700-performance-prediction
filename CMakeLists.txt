project(gsat)

cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/gsat/cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Catch REQUIRED)

include(gsat/cmake/TestSolution.cmake)
include(gsat/cmake/BuildFlags.cmake)

include_directories(gsat/common)
include_directories(gsat/test)
add_subdirectory(gsat/test/map)

add_subdirectory(gsat/ds)
