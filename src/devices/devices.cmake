set(PCEM_SRC ${PCEM_SRC}
        devices/device.c
        devices/esdi_at.c
        devices/f82c710_upc.c
        devices/nvr.c
        devices/ps2_nvr.c
        devices/sis496.c

        # TODO: Remove sound_ prefix from the file names,
        # it's kinda pointless!
        devices/sound/sound_ad1848.c
        devices/sound/sound_adlib.c
        devices/sound/sound_adlibgold.c
        devices/sound/sound_audiopci.c
        devices/sound/sound_azt2316a.c
        devices/sound/sound_cms.c
        devices/sound/sound_dbopl.cc
        devices/sound/sound_emu8k.c
        devices/sound/sound_gus.c
        devices/sound/sound_mmb.c
        devices/sound/sound_mpu401_uart.c
        devices/sound/sound_opl.c
        devices/sound/sound_pas16.c
        devices/sound/sound_ps1.c
        devices/sound/sound_resid.cc
        devices/sound/sound_sb.c
        devices/sound/sound_sb_dsp.c
        devices/sound/sound_sn76489.c
        devices/sound/sound_speaker.c
        devices/sound/sound_ssi2001.c
        devices/sound/sound_wss.c
        devices/sound/sound_ym7128.c
        )
