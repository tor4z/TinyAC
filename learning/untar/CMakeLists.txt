project(untar)


add_executable(untar
    main.c
)

target_link_libraries(untar
    libarchive.a
)
