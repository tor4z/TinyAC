file(GLOB SRCS *.cpp)
file(GLOB C_SRCS *.c)
list(APPEND SRCS ${C_SRCS})


add_executable(${PROJECT_NAME}
    ${SRCS}
)
