set(PCEM_SRC ${PCEM_SRC}
        cdrom/cdrom-image.cc
        cdrom/cdrom-null.c
        )

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(PCEM_SRC ${PCEM_SRC} ${PCEM_SRC_CDROM}
                cdrom/cdrom-ioctl-linux.c
                )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
        set(PCEM_SRC ${PCEM_SRC} ${PCEM_SRC_CDROM}
                cdrom/cdrom-ioctl.c
                )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
        set(PCEM_SRC ${PCEM_SRC} ${PCEM_SRC_CDROM}
                cdrom/cdrom-ioctl-osx.c
                )
endif()

