# Third-party libraries.

add_library(3rdparty-minivhd
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/convert.c
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/create.c
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/cwalk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/io.c
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/manage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/struct_rw.c
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/util.c
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src/xml2_encoding.c
)

target_include_directories(3rdparty-minivhd PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/minivhd/src
)

add_subdirectory(dosbox)
