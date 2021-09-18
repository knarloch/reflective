cmake_minimum_required(VERSION 3.20)
project(reflective)

set(CMAKE_CXX_STANDARD 20)
enable_testing()

include_directories(3rdParty/json/single_include)

add_subdirectory(3rdParty)
add_subdirectory(test)





