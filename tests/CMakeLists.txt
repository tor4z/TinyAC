project(TinyACTest)

find_package(GTest REQUIRED)


file(GLOB TEST_SRCS *.cpp)


enable_testing()
add_executable(testTarget
    ${TEST_SRCS}
)

target_link_libraries(
    testTarget
    GTest::Main tac
)


include(GoogleTest)
gtest_discover_tests(testTarget)
