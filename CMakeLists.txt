cmake_minimum_required(VERSION 3.10)

project(solver LANGUAGES CXX)

add_subdirectory(src)
add_subdirectory(apps)
enable_testing()
add_subdirectory(tests)
