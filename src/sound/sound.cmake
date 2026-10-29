set(PCEM_SRC ${PCEM_SRC}
        sound/sound.c
        )

# TODO submodule in //3rdparty

# AYUMI
set(PCEM_SRC ${PCEM_SRC}
        sound/ayumi/ayumi.c
        )

# RESID-FP
set(PCEM_SRC ${PCEM_SRC}
        sound/resid-fp/convolve.cc
        sound/resid-fp/convolve-sse.cc
        sound/resid-fp/envelope.cc
        sound/resid-fp/extfilt.cc
        sound/resid-fp/filter.cc
        sound/resid-fp/pot.cc
        sound/resid-fp/sid.cc
        sound/resid-fp/voice.cc
        sound/resid-fp/wave6581_PS_.cc
        sound/resid-fp/wave6581_PST.cc
        sound/resid-fp/wave6581_P_T.cc
        sound/resid-fp/wave6581__ST.cc
        sound/resid-fp/wave8580_PS_.cc
        sound/resid-fp/wave8580_PST.cc
        sound/resid-fp/wave8580_P_T.cc
        sound/resid-fp/wave8580__ST.cc
        sound/resid-fp/wave.cc
        )

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(PCEM_SRC ${PCEM_SRC}
                sound/midi_alsa.c
                )
        set(PCEM_ADDITIONAL_LIBS ${PCEM_ADDITIONAL_LIBS} ${ALSA_LIBRARIES})
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(PCEM_SRC ${PCEM_SRC}
                sound/win-midi.c
                )
                set(PCEM_ADDITIONAL_LIBS ${PCEM_ADDITIONAL_LIBS} winmm)
endif()
