# TODO: Gradually remove this
set(PCEM_PUBLIC_API ${PCEM_PUBLIC_API}
        ${CMAKE_SOURCE_DIR}/includes/public/pcem/api.h
        ${CMAKE_SOURCE_DIR}/includes/public/pcem/cpu.h
        ${CMAKE_SOURCE_DIR}/includes/public/pcem/defines.h
        ${CMAKE_SOURCE_DIR}/includes/public/pcem/devices.h
        ${CMAKE_SOURCE_DIR}/includes/public/pcem/logging.h
        ${CMAKE_SOURCE_DIR}/includes/public/pcem/config.h
        )

set(PCEM_SRC_PLUGINAPI
        ${PCEM_SRC_PLUGINAPI}
        plugin-api/paths.c
        plugin-api/logging.c
        #plugin-api/device.c
        plugin-api/plugin.c
        )

if(${PCEM_DISPLAY_ENGINE} STREQUAL "wxWidgets")
        set(PCEM_SRC_PLUGINAPI ${PCEM_SRC_PLUGINAPI} plugin-api/wx-utils.cc)
endif()

set(PCEM_EMBEDDED_PLUGIN_API ${PCEM_SRC_PLUGINAPI} ${PCEM_PUBLIC_API})
