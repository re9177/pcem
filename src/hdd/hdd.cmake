set(PCEM_SRC ${PCEM_SRC}
        hdd/hdd.c
        hdd/hdd_esdi.c
        hdd/hdd_file.c
        )

# RAMDisk
set(PCEM_SRC ${PCEM_SRC}
        hdd/ramdisk/ramdisk.c
        )

# MiniVHD
set(PCEM_SRC ${PCEM_SRC}
        hdd/minivhd/cwalk.c
        hdd/minivhd/libxml2_encoding.c
        hdd/minivhd/minivhd_convert.c
        hdd/minivhd/minivhd_create.c
        hdd/minivhd/minivhd_io.c
        hdd/minivhd/minivhd_manage.c
        hdd/minivhd/minivhd_struct_rw.c
        hdd/minivhd/minivhd_util.c
        )
