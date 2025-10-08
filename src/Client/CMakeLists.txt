cmake_minimum_required(VERSION 3.28)
project(low-level_Client
 VERSION 1.0
 LANGUAGES CXX
)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(dependencies)

add_executable(${PROJECT_NAME} 
	main.cpp
	"ChatClient.hpp"
)

find_package(cppzmq)
#find cppzmq wrapper, installed by make of cppzmq
target_link_libraries(${PROJECT_NAME} cppzmq)
# Or use static library to link
#target_link_libraries(${PROJECT_NAME} cppzmq-static)