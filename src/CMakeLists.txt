find_package(Threads REQUIRED)
add_library(solver STATIC solver/dancing_links.cpp solver/option_handler.cpp)
target_link_libraries(solver PRIVATE Threads::Threads)
target_include_directories(solver PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(solver PUBLIC cxx_std_20)
target_compile_options(solver PUBLIC -Wall -Wextra)
