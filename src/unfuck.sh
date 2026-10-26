#for f in $(rg '\#include \"device.h\"' -l); do sed -i 's/\"device.h\"/\"plugin-api\/device.h\" \/\/\ REMOVE/' $f && echo $f; done
#for f in $(rg '\#include \"config.h\"' -l); do sed -i 's/\"config.h\"/\"plugin-api\/config.h\" \/\/\ REMOVE/' $f && echo $f; done

for f in $(rg '\#include \"plugin-api/device.h\"' -l); do sed -i 's/\"plugin-api\/device.h\"/\"devices\/device.h\"/' $f && echo $f; done
#for f in $(rg '\#include \"plugin-api/config.h\"' -l); do sed -i 's/\"plugin-api\/config.h\"/\"config.h\"/' $f && echo $f; done

# bus/
for f in $(rg '\#include \"pci.h\"' -l); do sed -i 's/\"pci.h\"/\"bus\/pci.h\"/' $f && echo $f; done
for f in $(rg '\#include \"mca.h\"' -l); do sed -i 's/\"mca.h\"/\"bus\/mca.h\"/' $f && echo $f; done

# cpu/
for f in $(rg '\#include \"cpu.h\"' -l); do sed -i 's/\"cpu.h\"/\"cpu\/cpu.h\"/' $f && echo $f; done
for f in $(rg '\#include \"x86.h\"' -l); do sed -i 's/\"x86.h\"/\"cpu\/x86.h\"/' $f && echo $f; done

# devices
for f in $(rg '\#include \"cassette.h\"' -l); do sed -i 's/\"cassette.h\"/\"devices\/cassette.h\"/' $f && echo $f; done
for f in $(rg '\#include \"ps2_nvr.h\"' -l); do sed -i 's/\"ps2_nvr.h\"/\"devices\/ps2_nvr.h\"/' $f && echo $f; done
for f in $(rg '\#include \"nvr.h\"' -l); do sed -i 's/\"nvr.h\"/\"devices\/nvr.h\"/' $f && echo $f; done


# flash
for f in $(rg '\#include \"rom.h\"' -l); do sed -i 's/\"rom.h\"/\"flash\/rom.h\"/' $f && echo $f; done

# memory/
for f in $(rg '\#include \"mem.h\"' -l); do sed -i 's/\"mem.h\"/\"memory\/mem.h\"/' $f && echo $f; done

# models/ move
for f in $(rg '\#include \"dma.h\"' -l); do sed -i 's/\"dma.h\"/\"models\/dma.h\"/' $f && echo $f; done
for f in $(rg '\#include \"pic.h\"' -l); do sed -i 's/\"pic.h\"/\"models\/pic.h\"/' $f && echo $f; done
for f in $(rg '\#include \"pit.h\"' -l); do sed -i 's/\"pit.h\"/\"models\/pit.h\"/' $f && echo $f; done
# fuck youy manley
for f in $(rg '\#include \"serial.h\"' -l); do sed -i 's/\"serial.h\"/\"models\/serial.h\"/' $f && echo $f; done
for f in $(rg '\#include \"nmi.h\"' -l); do sed -i 's/\"nmi.h\"/\"models\/nmi.h\"/' $f && echo $f; done


# floppy/
for f in $(rg '\#include \"fdc.h\"' -l); do sed -i 's/\"fdc.h\"/\"floppy\/fdc.h\"/' $f && echo $f; done
for f in $(rg '\#include \"fdd.h\"' -l); do sed -i 's/\"fdd.h\"/\"floppy\/fdd.h\"/' $f && echo $f; done

# keyboard/
for f in $(rg '\#include \"keyboard.h\"' -l); do sed -i 's/\"keyboard.h\"/\"keyboard\/keyboard.h\"/' $f && echo $f; done
for f in $(rg '\#include \"keyboard_at.h\"' -l); do sed -i 's/\"keyboard_at.h\"/\"keyboard\/keyboard_at.h\"/' $f && echo $f; done

# mouse/
for f in $(rg '\#include \"mouse.h\"' -l); do sed -i 's/\"mouse.h\"/\"mouse\/mouse.h\"/' $f && echo $f; done

# lpt/
for f in $(rg '\#include \"lpt.h\"' -l); do sed -i 's/\"lpt.h\"/\"lpt\/lpt.h\"/' $f && echo $f; done

# serial MOVE
for f in $(rg '\#include \"serial.h\"' -l); do sed -i 's/\"serial.h\"/\"models\/serial.h\"/' $f && echo $f; done

# ide/
for f in $(rg '\#include \"ide.h\"' -l); do sed -i 's/\"ide.h\"/\"ide\/ide.h\"/' $f && echo $f; done

# video
for f in $(rg '\#include \"video.h\"' -l); do sed -i 's/\"video.h\"/\"video\/video.h\"/' $f && echo $f; done

