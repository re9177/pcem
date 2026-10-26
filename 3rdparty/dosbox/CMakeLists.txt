add_library(3rdparty-dosbox
    src/cdrom_image.cpp
    src/dbopl.cpp
    src/nukedopl.cpp
    src/vid_cga_comp.c
)

# Code depends on pcem includes
target_include_directories(3rdparty-dosbox PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_include_directories(3rdparty-dosbox PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
