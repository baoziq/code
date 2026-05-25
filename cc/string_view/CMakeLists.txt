cmake_minimum_required(VERSION 3.10)

project(StringViewTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(test_string_view
    test_string_view.cc
    string_view.cc
)

target_compile_options(test_string_view PRIVATE
    -Wall
    -Wextra
    -Werror
    -g
)
