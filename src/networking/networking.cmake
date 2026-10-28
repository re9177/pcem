set(PCEM_DEFINES ${PCEM_DEFINES} USE_NETWORKING)

# TODO
#find_package(PkgConfig REQUIRED)
#pkg_check_modules(SLIRP REQUIRED slirp)
#include_directories(${SLIRP_INCLUDE_DIRS})
#set(PCEM_ADDITIONAL_LIBS ${PCEM_ADDITIONAL_LIBS} ${SLIRP_LIBRARIES})

if(USE_PCAP_NETWORKING)
        set(PCEM_DEFINES ${PCEM_DEFINES} USE_PCAP_NETWORKING)
        set(PCEM_ADDITIONAL_LIBS ${PCEM_ADDITIONAL_LIBS} ${PCAP_LIBRARY})
endif()

set(PCEM_SRC ${PCEM_SRC}
        networking/nethandler.c

        networking/netbackend.c
        networking/netpacket.cpp
        networking/netbackend_null.cpp
        networking/netbackend_tap_linux.cpp
        #networking/queue.c
        )

if(WIN32)
        set(PCEM_ADDITIONAL_LIBS ${PCEM_ADDITIONAL_LIBS} wsock32 iphlpapi ws2_32)
endif()
