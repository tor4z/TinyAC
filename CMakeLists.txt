cmake_minimum_required(VERSION 3.10)

project(tinyAC VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DNDEBUG)
endif()

include_directories(include)


# config version
configure_file(
    ${CMAKE_CURRENT_LIST_DIR}/include/tac/version.hpp.in
    ${CMAKE_CURRENT_LIST_DIR}/include/tac/version.hpp
)

add_subdirectory(learning)
add_subdirectory(src)
add_subdirectory(tests)
