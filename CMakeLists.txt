cmake_minimum_required(VERSION 3.2)

project(dancing_links LANGUAGES CXX)

add_executable(dancing_links dancing_links.cpp)
target_compile_features(dancing_links PUBLIC cxx_std_17)
target_compile_options(dancing_links PUBLIC -O2 -Wall)
