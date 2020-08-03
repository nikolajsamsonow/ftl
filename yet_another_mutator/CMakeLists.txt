cmake_minimum_required(VERSION 3.17)
project(yet_another_mutator)

set(CMAKE_CXX_STANDARD 14)

add_library(yet_another_mutator SHARED library.cc library.h yam.cc yam.h)
