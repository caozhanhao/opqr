cmake_minimum_required(VERSION 3.8)
project(opqr)

set(CMAKE_CXX_STANDARD 17)
include_directories(include)
add_executable(opqr src/main.cpp)
#SET_TARGET_PROPERTIES(opqr PROPERTIES LINK_FLAGS "/PROFILE")

