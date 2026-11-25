include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/a46a6012a133c7c0929fdae55b321c2fe65f3c5f.zip
)
FetchContent_MakeAvailable(googletest)

set(NAME "${CMAKE_PROJECT_NAME}_tests")
add_executable(${NAME} test_tests.cpp)
target_link_libraries(${NAME} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${NAME} DISCOVERY_MODE PRE_TEST)
