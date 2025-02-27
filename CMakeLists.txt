cmake_minimum_required(VERSION 3.30)
project(lm)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
        ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
        GIT_TAG "main"
        GIT_PROGRESS TRUE
        GIT_SHALLOW 1
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_MakeAvailable(ftxui)
endif()

include_directories(vendor)

add_executable(lm main.cpp
        config.cpp
        config.h)

target_link_libraries(lm
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
)
