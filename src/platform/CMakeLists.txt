add_library(pcem-platform
)

if(PCEM_PLATFORM STREQUAL "sdl2")
target_sources(pcem-platform PRIVATE
	sdl2-keyboard.c
	sdl2-mouse.c
	sdl2-joystick.c
)
else()
message(FATAL_ERROR "Unknown platform ${PCEM_PLATFORM}")
endif()

target_include_directories(pcem-platform PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
