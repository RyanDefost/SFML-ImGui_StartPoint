cmake_minimum_required(VERSION 3.28)

project(low-level_setup
 VERSION 1.0
 LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20) 

add_subdirectory(dependencies)
add_subdirectory(src)