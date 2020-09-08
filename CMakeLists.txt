cmake_minimum_required (VERSION 2.8)

project(RTR_Demo)

add_subdirectory(glfw-3.3.2)

add_subdirectory(Core)

message(OutputDir is： ${PROJECT_BINARY_DIR})