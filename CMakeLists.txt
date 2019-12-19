cmake_minimum_required(VERSION 2.8)

project(3d_converted)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 5.1)
    message(FATAL_ERROR "Require at least gcc-5.1")
endif()
add_executable(${PROJECT_NAME} "main.cpp")
