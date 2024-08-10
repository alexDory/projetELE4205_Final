cmake_minimum_required(VERSION 2.8.12)
project(livrable)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurer le projet en mode débogage.
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDebug")



add_executable(client client.cpp)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS}
/usr/include/tesseract
/usr/include/leptonica)
target_link_libraries(client ${OpenCV_LIBRARIES}
tesseract lept)

