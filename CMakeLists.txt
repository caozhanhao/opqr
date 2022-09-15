cmake_minimum_required(VERSION 3.8)
project(opqr)

set(CMAKE_CXX_STANDARD 17)
include(include/opqr)
add_executable(opqr src/main.cpp)
