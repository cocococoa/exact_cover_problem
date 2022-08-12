cmake_minimum_required(VERSION 3.2)

project(dancing_links LANGUAGES CXX)

add_library(dancing_links STATIC dancing_links.cpp)
target_include_directories(dancing_links PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(dancing_links PUBLIC cxx_std_17)
target_compile_options(dancing_links PUBLIC -O2 -Wall -Wextra)
target_compile_definitions(dancing_links PUBLIC NDEBUG)

foreach(exe example langford)
    add_executable(${exe} ${exe}.cpp)
    target_link_libraries(${exe} PRIVATE dancing_links)
endforeach()
