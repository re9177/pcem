# REWORK LIST

This is not really in order of importance but generally tracks things

- [] Reorganize devices into src/devices folder.
	- Per category. 
		- e.g: network devices go into src/devices/network, sound src/devices/sound etc etc.

- [] Rewrite config entirely
	- xml or some sort of actually structured configuration language
	- see ./new-config-example.xml for examples of how it might look in XML

- [] Reimplement src/pc.c as a Machine c++ class
	- [] Make busses part of the machine, rather than globals
	- [] Devices/config added as tree
	- [] Eliminiate machine-specific globals, so multiple machines can run at the same time in one process
	
- [] Make devices C++ classes

- [] Rework how platforms work. UI doesn't particuarly need to own platform implementation,
 unless it itself is implementing it. For now I'm keeping SDL2 going but when Qt is up we could
 look into doing things better then

- [] Rewrite audio and video backend entirely
	- probably as a part of platform/machine rework
	
- [] New Qt UI
