cmake_minimum_required(VERSION 3.2)

project(dancing_links LANGUAGES CXX)

foreach(exe example langford)
    add_executable(${exe} ${exe}.cpp)
    target_include_directories(${exe} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_features(${exe} PUBLIC cxx_std_17)
    target_compile_options(${exe} PUBLIC -O2 -Wall -Wextra)
    target_compile_definitions(${exe} PUBLIC NDEBUG)
endforeach()
