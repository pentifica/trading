cmake_minimum_required(VERSION 3.22)

project(
    trading
    VERSION 1.0
    DESCRIPTION "A trading components"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(src)
add_subdirectory(tests)
