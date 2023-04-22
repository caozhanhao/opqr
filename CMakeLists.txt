cmake_minimum_required(VERSION 3.8.2)
project(opqr)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
add_executable(opqr examples/src/main.cpp)
#SET_TARGET_PROPERTIES(opqr PROPERTIES LINK_FLAGS "/PROFILE")

