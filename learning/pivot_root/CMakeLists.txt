project(pivot_root)


add_executable(pivot_root
    main.c
)


install(
    FILES run.sh DESTINATION ${PROJECT_BINARY_DIR}
)
