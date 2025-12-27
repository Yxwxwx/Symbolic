cmake_minimum_required(VERSION 3.28)

set(PROJECT_NAME "Symbolic")
set(PROJECT_VERSION "0.1.0")

project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "Configuring project: ${PROJECT_NAME} version ${PROJECT_VERSION}"
  " with C++ standard ${CMAKE_CXX_STANDARD}")
message(STATUS "Using C++ compiler: ${CMAKE_CXX_COMPILER}")

add_executable(${PROJECT_NAME}_exe src/main.cpp)