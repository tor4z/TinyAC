project(untar)


add_executable(untar
    untar.c
)

target_link_libraries(untar
    libarchive.a
)


add_executable(tar
    tar.c
)

target_link_libraries(tar
    libarchive.a
)
