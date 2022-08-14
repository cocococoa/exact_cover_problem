foreach(exe example langford pentominoes latin)
    add_executable(${exe} ${exe}.cpp)
    target_link_libraries(${exe} PRIVATE dancing_links)
endforeach()
