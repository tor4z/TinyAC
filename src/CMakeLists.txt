file(GLOB SRCS *.cpp)
file(GLOB C_SRCS *.c)
list(APPEND SRCS ${C_SRCS})


add_executable(${PROJECT_NAME}
    ${SRCS}
)

target_link_libraries(${PROJECT_NAME}
    libarchive.a
)


# for unit test
set(SRCS_WO_MAIN ${SRCS})
list(REMOVE_ITEM ${SRCS_WO_MAIN} main.cpp)

add_library(tac
    ${SRCS_WO_MAIN}
)

target_link_libraries(tac
    libarchive.a
)
