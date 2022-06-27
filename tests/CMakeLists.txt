project(TinyACTest)

find_package(GTest REQUIRED)


file(GLOB TEST_SRCS *.cpp)

enable_testing()
add_executable(tests
    ${TEST_SRCS}
)

target_link_libraries(
    tests
    GTest::Main
)

include(GoogleTest)
gtest_discover_tests(tests)
