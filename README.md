# PCem

This is a hard fork of PCem. The authors feel that:
- The AI policy was shoe-horned in by the lead maintainer to permit AI generated code in several places where it was forcefully introduced without acknowledgement.
  - The plugin API, NE2000, and Qt user interface (before its removal) were blatantly vibecoded.
- PC and AT emulation is better covered by less hacky emulators.
- The upstream PCem codebase is simply too annoying to work on.

We may follow upstream in some cases, but generally, this fork is intended to evolve seperately. We will not make any major choices to make
syncing with upstream easier, as that is not the goal of this fork.

We are currently mainly focusing on heavily refactoring things behind the scenes, but we will be working on several new features,
and new emulated machines.

Pull requests are welcomed. They will be reviewed and accepted if they are deemed valuable. Do note that we **will** refuse any AI generated pull requests.

Latest version: <b>v17</b> [Changelog](CHANGELOG.md)

PCem is licensed under GPL v2.0, see [COPYING](COPYING) for more details.

:exclamation: Note: <b>NO COPYRIGHTED ROM FILES ARE INCLUDED NOR WILL THEY BE. PLEASE DO NOT ASK FOR THEM.</b>

## BSD and Linux supplement

You will need the following libraries and buildtools (and their dependencies):
- SDL2
- wxWidgets 3.x
- OpenAL
- CMake
- Ninja (Recommended, but you can use a Makefile generator if you prefer)
- CLang Toolchain

Open a terminal window, navigate to the PCem directory, then enter:
### Linux/BSD
```
cmake -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
ninja
```

then `build/src/pcem` to run.

BIOS ROM images, configuration files, and other data are stored in `~/.pcem`. You can also create a `.pcem` folder with
the Binary, and run it in a portable mode.

The configure options are specified below. They are in the format of -D`Option`=`Value`. `Value` under here is the
default value.
```
  -DCMAKE_BUILD_TYPE=Release : Generate release build. Recommended for regular use.
  -DCMAKE_BUILD_TYPE=Debug   : Compile with debugging enabled.
  -DPCEM_DISPLAY_ENGINE=wxWidgets : Compile with the WxWidgets Display Engine. Default
  -DPCEM_DISPLAY_ENGINE=Qt   : Compile with the Qt Display Engine.
  -DUSE_NETWORKING=ON        : Build with networking support.
  -DUSE_PCAP_NETWORKING=ON   : Build with pcap networking support. (Needs USE_NETWORKING to compile) Requires libpcap.
  -DUSE_ALSA=OFF             : Build with support for MIDI output through ALSA. Requires libasound. (Linux Only)
  -DFORCE_X11=ON             : Enables a hack to force X11 on Wayland systems. See #128 for details. (Linux Only)
```

If you are using -DCMAKE_BUILD_TYPE=Debug, there are some more debug options you can enable if needed
```
  -DPCEM_SLIRP_DEBUG=OFF           : Build PCem with SLIRP_DEBUG debug output
  -DPCEM_RECOMPILER_DEBUG=OFF      : Build PCem with RECOMPILER_DEBUG debug output
  -DPCEM_NE2000_DEBUG=OFF          : Build PCem with NE2000_DEBUG debug output
  -DPCEM_EMU8K_DEBUG_REGISTERS=OFF : Build PCem with EMU8K_DEBUG_REGISTERS debug output
  -DPCEM_SB_DSP_RECORD_DEBUG=OFF   : Build PCem with SB_DSP_RECORD_DEBUG debug output
  -DPCEM_MACH64_DEBUG=OFF          : Build PCem with MACH64_DEBUG debug output
  -DPCEM_DEBUG_EXTRA=OFF           : Build PCem with DEBUG_EXTRA debug output
```

If you are using -DCMAKE_BUILD_TYPE=RelWithDebInfo, there are additional options you can do
```
  -DPCEM_RELDEB_AS_RELEASE=ON     : Builds RelWithDebInfo with debugging logging enabled when this is off
```

They are some extra modules you can add if you build with `-DUSE_EXPERIMENTAL=ON`. These modules are untested.
incomplete, and may or may not be in a future build of PCem. We do not provide builds with these enabled as
well. It is also possible they may not even build.
```
  -DUSE_EXPERIMENTAL_PGC=ON       : Build PCem with Professional Graphics Controller support.
``` 
The menu is a pop-up menu in the Linux/BSD port. Right-click on the main window when mouse is not
captured.

CD-ROM support currently only accesses `/dev/cdrom`. It has not been heavily tested.

On Windows, if you want to use PCAP networking, you will need to install [npcap](https://npcap.com) and replace the libpcap.dll in PCem dir using the wpcap.dll installed in C:\Windows\System32

On Linux, if you want to use PCap Networking, run `setcap cap_net_raw,cap_net_admin=eip (path to binary here)` as root.

## Links

### PCem emulates the following hardware:

Hardware | Links
--- | ---
Systems | [8088](#8088-based)<br/>[8086](#8086-based)<br/>[286](#286-based)<br/>[386](#386-based)<br/>[486](#486-based)<br/>[Pentium](#pentium-based)<br/>[Super Socket 7](#super-socket-7-based)
Graphics | [Basic](#basic-cards)<br/>[Unaccelerated (S)VGA cards](#unaccelerated-svga-cards)<br/>[2D accelerated SVGA cards](#2d-accelerated-svga-cards)<br/>[3D accelerated SVGA cards](#3d-accelerated-svga-cards)<br/>[3D only cards](#3d-only-cards)
Sound | [Cards](#sound-cards)
HDD Controller | [Cards](#hdd-controller-cards)
Misc | [Cards](#misc-cards)

### [Software Tested](TESTED.md) (list)
- [DOS](TESTED.md#dos)<br/>
- [Windows](TESTED.md#windows)<br/>
- [Windows NT](TESTED.md#windows-nt)<br/>
- [OS/2](TESTED.md#os2)<br/>
- [Linux](TESTED.md#linux)<br/>
- [Applications](TESTED.md#applications)<br/>
- [Games](TESTED.md#games)<br/>
- [Demos](TESTED.md#demos)<br/>
- [Emulators](TESTED.md#emulators)<br/>

<hr>
