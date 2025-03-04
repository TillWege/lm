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

FetchContent_MakeAvailable(ftxui)

FetchContent_Declare(
        SqliteCpp
        GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
        GIT_TAG "master"
        GIT_PROGRESS TRUE
        GIT_SHALLOW 1
)

FetchContent_MakeAvailable(SqliteCpp)

include_directories(vendor)

add_executable(lm main.cpp
        config.cpp
        config.h
        interface.h
        interface.cpp
        storage.h
        storage.cpp
)

target_link_libraries(lm
    PRIVATE ftxui::screen
    PRIVATE ftxui::dom
    PRIVATE ftxui::component
    SQLiteCpp sqlite3
)

if (MSVC)
    target_compile_options(lm PRIVATE /W4 /WX)
else()
    target_compile_options(lm PRIVATE -Wall -Wextra -Wpedantic)
    target_link_libraries(lm PRIVATE pthread dl)
endif ()
