cmake_minimum_required(VERSION 2.8.12)
project(server)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDebug")
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable(server serveur.cpp)



find_package(OpenCV REQUIRED)


target_link_libraries(server ${OpenCV_LIBRARIES})

