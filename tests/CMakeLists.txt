include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(xc_test xc_test.cpp)
target_link_libraries(xc_test solver gtest_main)
gtest_discover_tests(xc_test)

add_executable(xcc_test xcc_test.cpp)
target_link_libraries(xcc_test solver gtest_main)
gtest_discover_tests(xcc_test)
