cmake_minimum_required(VERSION 3.0.0)
project(hamarc VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wuninitialized -Wno-unused-result -Wshadow")

add_subdirectory(bin)
add_subdirectory(lib)
