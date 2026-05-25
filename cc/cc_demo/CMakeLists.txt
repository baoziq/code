cmake_minimum_required(VERSION 3.22.1)
project(cc)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
add_executable(demo demo.cc)

set_property(TARGET demo PROPERTY CXX_STANDARD 23)
set_property(TARGET demo PROPERTY CXX_STANDARD_REQUIRED ON)