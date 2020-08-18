cmake_minimum_required(VERSION 3.16)
project(yet_another_mutator)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

add_library(yet_another_mutator SHARED library.cc library.h yam.cc yam.h)
