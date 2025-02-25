cmake_minimum_required(VERSION 3.30)
project(lm)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(vendor)

add_executable(lm main.cpp
        config.cpp
        config.h)
