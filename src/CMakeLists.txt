if(FORCE_X11)
        set(PCEM_DEFINES ${PCEM_DEFINES} FORCE_X11)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/bus/bus.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cdrom/cdrom.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/codegen/codegen.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cpu/cpu.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/devices/devices.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/disc/disc.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/dosbox/dosbox.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/flash/flash.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/floppy/floppy.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/hdd/hdd.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/ide/ide.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/joystick/joystick.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/keyboard/keyboard.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/lpt/lpt.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/memory/memory.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/mfm/mfm.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/models/models.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/mouse/mouse.cmake)
if(USE_NETWORKING)
        include(${CMAKE_CURRENT_SOURCE_DIR}/networking/networking.cmake)
endif()
include(${CMAKE_CURRENT_SOURCE_DIR}/scsi/scsi.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/sound/sound.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/video/video.cmake)

if(${PCEM_DISPLAY_ENGINE} STREQUAL "wxWidgets")
        include(${CMAKE_CURRENT_SOURCE_DIR}/wx-ui/wx-ui.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/wx-ui/viewers/viewers.cmake)
endif()

set(PCEM_SRC ${PCEM_SRC}
        config.c
        logging.c
        fdi2raw.c
        io.c
        mcr.c
        pc.c
        ppi.c
        pzx.c
        rtc.c
        rtc_tc8521.c
        timer.c
        )

set(PCEM_LIBRARIES ${DISPLAY_ENGINE_LIBRARIES} ${SDL2_LIBRARIES} ${OPENAL_LIBRARY} ${OPENGL_LIBRARIES} ${PCEM_ADDITIONAL_LIBS})

include(${CMAKE_CURRENT_SOURCE_DIR}/plugin-api/plugin-api.cmake)

set(PCEM_LIBRARIES ${PCEM_LIBRARIES} PARENT_SCOPE)
if(${PCEM_DISPLAY_ENGINE} STREQUAL "wxWidgets")
add_executable(pcem ${PCEM_SRC} ${PCEM_EMBEDDED_PLUGIN_API})
endif()
# Pull in private includes
target_include_directories(pcem PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        # MiniVHD - TODO Submodule
        ${PROJECT_SOURCE_DIR}/src/hdd/
        # Ayumi - TODO submodule
        ${PROJECT_SOURCE_DIR}/src/sound/
)

target_compile_definitions(pcem PUBLIC ${PCEM_DEFINES})
target_compile_options(pcem PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-common> $<$<COMPILE_LANGUAGE:C>:-fno-common>)
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        target_link_options(pcem PRIVATE "-Wl,--subsystem,windows")
endif()
target_link_libraries(pcem PRIVATE ${PCEM_LIBRARIES})
