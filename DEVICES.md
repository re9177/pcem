# PCem Devices

## Graphics Cards

### Basic cards
Hardware | Addl. Info | ROM file needed<br/>(within ./roms/ folder)
--- | --- | ---
<b>MDA</b> <i>(1981)</i> | 80x25 monochrome text | <i>(none)</i>
<b>CGA</b> <i>(1981)</i> | 40x25 and 80x25 text<br/>320x200 in 4 colours<br/>620x200 in 2 colours<br/>Supports composite output for ~16 colours. | <i>(none)</i>
<b>Hercules</b> <i>(1982)</i> | 80x25 monochrome text<br/>720x348 in monochrome | <i>(none)</i>
<b>Plantronics ColorPlus</b> | An enhanced CGA board with support for 320x200x16 and 640x200x4 | <i>(none)</i>
<b>Wyse WY-700</b> | A CGA-compatible board with support for a 1280x800 mode | <i>(none)</i>
<b>MDSI Genius</b> | Mono portrait board with support for a 728x1008 mode | 8x12.bin
<b>IBM EGA</b> <i>(1984)</i> | Text up to 80x43<br/>Graphics up to 640x350 in 16 colours | ibm_6277356_ega_card_u44_27128.bin
<b>ATI EGA Wonder 800+</b> | An enhanced EGA-compatible board with support for up to 800x600 in 16 colours | ATI EGA Wonder 800+ N1.00.BIN
<b>Hercules InColor</b> | An enhanced Hercules with a custom 720x350 16 colour mode | <i>(none)</i>
<b>Yamaha V6335D</b> | A CGA-compatible chip originally for laptops with LCDs, also used in cards like the ACV-1030 and systems such as the Olivetti Prodest PC1. | <i>(none)</i>

### Unaccelerated (S)VGA cards
Hardware | Addl. Info | ROM file needed<br/>(within ./roms/ folder)
--- | --- | ---
<b>ATI Korean VGA</b> | ATI-28800 based.<br/>512KB VRAM<br/>Supports up to 8-bit colour<br/>Korean font support | atikorvga.bin<br/>ati_ksc5601.rom
<b>ATI VGA Edge-16</b> | ATI-18800 based<br/>512KB VRAM<br/>Supports up to 8-bit colour | vgaedge16.vbi
<b>ATI VGA Charger</b> | ATI-28800 based<br/>512KB VRAM<br/>Supports up to 8-bit colour | bios.bin
<b>AVGA2</b> | Also known as Cirrus Logic GD5402<br/>256KB - 512KB VRAM<br/>Supports up to 8-bit colour | avga2vram.vbi
<b>IBM VGA</b> <i>(1987)</i> | 256KB VRAM<br/>Text up to 80x50<br/>Graphics up to 320x200 in 256 colours or 640x480 in 16 colours | ibm_vga.bin
<b>Kasan Hangulmadang-16</b> | ET4000AX based<br/>1MB VRAM<br/>Supports up to 8-bit colour<br/>Korean font support | et4000_kasan16.bin<br/>kasan_ksc5601.rom
<b>OAK OTI-037C</b> | 256KB VRAM<br/>Supports up to 8-bit colour | oti037/bios.bin
<b>OAK OTI-067</b> | 256KB - 512KB VRAM<br/>Supports up to 8-bit colour | oti067/bios.bin
<b>Olivetti GO481 (Paradise PVGA1A)</b> | 256KB VRAM<br/>Supports up to 8-bit colour | oli_go481_lo.bin<br/>oli_go481_hi.bin
<b>Trident 8900D SVGA</b> | 256KB - 1MB VRAM<br/>Supports up to 24-bit colour | trident.bin
<b>Trident 9000B SVGA</b> | 512KB VRAM<br/>Supports up to 8-bit colour | tvga9000b/BIOS.BIN
<b>Trident TGUI9400CXi</b> | 1MB - 2MB VRAM<br/>Supports up to 24-bit colour | 9440.vbi
<b>Trigem Korean VGA</b> | ET4000AX based<br/>1MB VRAM<br/>Supports up to 8-bit colour<br/>Korean font support | tgkorvga.bin<br/>tg_ksc5601.rom
<b>Tseng ET4000AX SVGA</b> | 1MB VRAM<br/>Supports up to 8-bit colour | et4000.bin

### 2D Accelerated SVGA cards
Hardware | Addl. Info | ROM file needed<br/>(within ./roms/ folder)
--- | --- | ---
<b>ATI Graphics Pro Turbo</b> | Mach64GX based<br/>1MB - 4MB VRAM | mach64gx/bios.bin
<b>ATI Video Xpression</b> | Mach64VT2 based<br/>2MB - 4MB VRAM<br/>Has video acceleration | atimach64vt2pci.bin
<b>Cirrus Logic GD-5428</b> | 1MB - 2MB VRAM | Machspeed_VGA_GUI_2100_VLB.vbi
<b>Cirrus Logic GD-5429</b> | 1MB - 2MB VRAM | 5429.vbi
<b>Cirrus Logic GD-5430</b> | 1MB - 2MB VRAM | gd5430/pci.bin
<b>Cirrus Logic GD-5434</b> <i>[[1]](#graphics-note-1)</i> | 2MB - 4MB VRAM | gd5434.bin
<b>Diamond Stealth 32 SVGA</b> | ET4000/W32p based<br/>1MB - 2MB VRAM | et4000w32.bin
<b>IBM 1MB SVGA Adapter/A</b> | Cirrus Logic GD5428 based<br/>1 MB VRAM<br/>Only supported on PS/2 models | SVGA141.ROM
<b>Number Nine 9FX</b> | S3 Trio64 based<br/>1MB - 2MB VRAM | s3_764.bin
<b>Paradise Bahamas 64</b> | S3 Vision864 based<br/>1MB - 4MB VRAM | bahamas64.bin
<b>Phoenix S3 Trio32</b> | S3 Trio32 based<br/>512KB - 2MB VRAM | 86c732p.bin
<b>Phoenix S3 Trio64</b> | S3 Trio64 based<br/>1MB - 4MB VRAM | 86c764x1.bin
<b>Trident TGUI9440</b> | 1MB - 2MB VRAM | 9440.vbi

### 3D Accelerated SVGA cards
Hardware | Addl. Info | ROM file needed<br/>(within ./roms/ folder)
--- | --- | ---
<b>3DFX Voodoo Banshee (reference)</b> | Voodoo Banshee based<br/>8MB - 16MB VRAM | pci_sg.rom
<b>3DFX Voodoo 3 2000</b> | Voodoo 3 based<br/>16MB VRAM | voodoo3_2000/2k11sd.rom
<b>3DFX Voodoo 3 3000</b> | Voodoo 3 based<br/>16MB VRAM | voodoo3_3000/3k12sd.rom
<b>Creative Labs 3D Blaster Banshee</b> | Voodoo Banshee based<br/>16MB VRAM | blasterpci.rom
<b>Diamond Stealth 3D 2000</b> <i>[[2]](#graphics-note-2)</i>| S3 ViRGE/325 based<br/>2MB - 4MB VRAM | s3virge.bin
<b>S3 ViRGE/DX</b> <i>[[3]](#graphics-note-3)</i>| S3 ViRGE/DX based<br/>2MB - 4MB VRAM | 86c375_1.bin

### 3D only cards
Hardware | Addl. Info 
--- | --- 
<b>3DFX Voodoo Graphics</b> <i>[[4]](#graphics-note-4)</i>| 3D accelerator. Widely supported in late 90s games.
<b>Obsidian SB50</b> <i>[[5]](#graphics-note-5)</i>| Voodoo with 2 TMUs
<b>3DFX Voodoo 2</b> <i>[[6]](#graphics-note-6)</i>| Improved Voodoo Graphics<br/>Higher clocks<br/>2 TMUs<br/>Triangle setup, etc.

#### Additional Notes
<a name="graphics-note-1">`[1]`</a> <b>Cirrus Logic GD-5434</b>: Real chip also supports 1MB configurations, however this is not currently supported in PCem.

<a name="graphics-note-2">`[2]`</a> <b>Diamond Stealth 3D 2000</b>: PCem emulates the ViRGE S3D engine in software. This works with most games I tried, but there may be some issues. The Direct3D drivers for the /325 are fairly poor (often showing as missing triangles), so use of the /DX instead is recommended.

<a name="graphics-note-3">`[3]`</a> <b>S3 ViRGE/DX</b>: The drivers that come with Windows are similar to those for the /325, however better ones do exist (try the 8-21-1997 version). With the correct drivers, many early Direct3D games work okay (if slowly).

<a name="graphics-note-4">`[4]`</a> <b>3DFX Voodoo Graphics</b>: PCem emulates this in software. The emulation is a lot faster than in v10 (thanks to a new dynamic recompiler) and should be capable of hitting Voodoo 1 performance on most machines when two render threads are used. As before, the emulated CPU is the bottleneck for most games. <br/><br/>PCem can emulate 6 and 8 MB configurations, but defaults to 4 MB for compatibility. It can also emulate the screen filter present on the original card, though this does at present have a noticeable performance hit.<br/><br/>Almost everything I've tried works okay, with a very few exceptions - Screamer 2 and Rally have serious issues.

<a name="graphics-note-5">`[5]`</a> <b>Obsidian SB50</b>: Drivers for this are a bit limited - the official drivers don't support 2 TMUs in Direct3D.

<a name="graphics-note-6">`[6]`</a> <b>3DFX Voodoo 2</b>: PCem can emulate both 8 and 12 MB configurations. It can also emulate 6 and 10 MB configurations (with 2MB framebuffer memory), which were never sold into the PC market but do exist for arcade systems.

<hr>

## Sound Cards

Hardware | Notes
--- | ---
<b>PC speaker</b> | The standard beeper on all PCs. Supports samples/RealSound.
<b>Tandy PSG</b> | The Texas Instruments chip in the PCjr and Tandy 1000. Supports 3 voices plus noise. I reused the emulator in B-em for this (slightly modified). PCem emulates the differences between the SN76496 (PCjr and Tandy 1000), and the NCR8496 (currently assigned to the Tandy 1000HX). Maniac Mansion and Zak McKraken will only sound correct on the latter.
<b>Tandy PSSJ</b> | Used on the Tandy 1000SL/2, this clones the NCR8496, adding an addition frequency divider (did any software actually use this?) and an 8-bit DAC.
<b>PS/1 audio card</b> | An SN76496 clone plus an 8-bit DAC. The SN76496 isn't at the same address as PCjr/Tandy, so most software doesn't support it.
<b>Gameblaster</b> | The Creative Labs Gameblaster/Creative Music System, Creative's first sound card introduced in 1987. Has two Philips SAA1099, giving 12 voices of square waves plus 4 noise voices. In stereo!
<b>Adlib</b> | Has a Yamaha YM3812, giving 9 voices of 2 op FM, or 6 voices plus a rhythm section. PCem uses the DOSBox dbopl emulator.
<b>Adlib Gold</b> | OPL3 with YM318Z 12-bit digital section. Possibly some bugs (not a lot of software to test). The surround module is now emulated.
<b>Sound Blaster</b> <i>[[1]](#sound-note-1)</i> | See linked note for more details.
<b>Gravis Ultrasound</b> | 32 voice sample playback. Port address is fixed to 240, IRQ and DMA can be changed from the drivers. Emulation is improved significantly over previous versions.
<b>Windows Sound System</b> | 16-bit digital + OPL3. Note that this only emulates WSS itself, and should not be used with drivers from compatible boards with additional components (eg Turtle Beach Monte Carlo)
<b>Aztech Sound Galaxy Pro 16 AB (Washington)</b> | SB compatible + WSS compatible
<b>Innovation SSI-2001</b> | SID6581. Emulated using resid-fp. Board is fixed to port 280.
<b>Ensoniq AudioPCI (ES1371)<br/>Sound Blaster PCI 128</b> | Basic PCI sound card. Emulates Sound Blaster in software.

### Additional Notes
<a name="sound-note-1">`[1]`</a> <b>Sound Blaster</b>: Several Sound Blasters are emulated.
* SB v1.0 - The original. Limited to 22khz, and no auto-init DMA (can cause crackles sometimes).
* SB v1.5 - Adds auto-init DMA
* SB v2.0 - Upped to 41khz
* SB Pro v1.0 - Stereo with twin OPL2 chips.
* SB Pro v2.0 - Stereo with OPL 3 chip
* SB 16 - 16 bit stereo
* SB AWE32 - SB 16 + wavetable MIDI. This requires a ROM dump from a real AWE32.

All cards are set to Address 220, IRQ 7 and DMA 1 (and High DMA 5). IRQ and DMA can be changed for the SB16 & AWE32 in the drivers. The relevant SET line for autoexec.bat is `SET BLASTER = A220 I7 D1 Tx` - where Tx is T1 for SB v1.0, T3 for SB v2.0, T4 for SB Pro, and T6 for SB16.

AWE32 requires a ROM dump called `awe32.raw`. AWE-DUMP is a utility which can get a dump from a real card. Most EMU8000 functionality should work, however filters are not correct and reverb/chorus effects are not currently emulated.

<hr>

## HDD Controller Cards

Int. | Hardware | Notes | ROM file needed<br/>(within ./roms/ folder)
:-: | --- | --- | ----
MFM | <b>Fixed Disk Adapter (Xebec)</b> | MFM controller originally included in the IBM XT. This controller only supports HD types 0, 2, 13, and 16. | ibm_xebec_62x0822_1985.bin
MFM | <b>DTC 5150X</b> | 8-bit MFM controller.<br/>To configure drive types, run `DEBUG.EXE` and enter `g=c800:5`. | dtc_cxd21a.bin
MFM | <b>AT Fixed Disk Adapter</b> | MFM controller originally included in the IBM AT. Supported by all AT-compatible BIOSes. | <i>(none)</i>
ESDI | <b>Western Digital WD1007V-SE1</b> | 16-bit ESDI controller | 62-000279-061.bin
ESDI | <b>IBM ESDI Fixed Disk Controller</b> | MCA ESDI controller. Only supported on PS/2 models. | 90x8969.bin<br/>90x8970.bin
IDE | <b>Standard IDE</b> | Standard IDE controller. Supported by all AT-compatible BIOSes. Use this if in any doubt! | <i>(none)</i>
IDE | <b>XTIDE</b> | 8-bit IDE controller. The BIOS is available [here](http://code.google.com/p/xtideuniversalbios/). <br/>v2.0.0 beta 1 is the version I've mostly tested. v2.0.0 beta 3 is known to have some issues. | ide_xt.bin
IDE | <b>XTIDE (AT)</b> | 16-bit IDE controller. | ide_at.bin
IDE | <b>XTIDE (PS/1)</b> | IDE controller for the PS/1 model 2033. For this machine you will need BIOS version v1.1.5. The PS/1 is a bit fussy with XTIDE, and I've found that it works best when the XTIDE configuration has 'Full Operating Mode' disabled. | ide_at_1_1_5.bin
SCSI | <b>Longshine LCS-6821N</b> | 8-bit SCSI controller. | Longshine LCS-6821N - BIOS version 1.04.bin
SCSI | <b>Rancho RT1000B</b> | 8-bit SCSI controller. | Rancho_RT1000_RTBios_version_8.10R.bin
SCSI | <b>Trantor T130B</b> | 8-bit SCSI controller. | trantor_t130b_bios_v2.14.bin
SCSI | <b>IBM SCSI Adapter with Cache</b> | MCA SCSI controller. Only supported on PS/2 models. | 92F2244.U68<br/>92F2245.U69
SCSI | <b>Adaptec AHA-1542C</b> | 16-bit SCSI controller. | adaptec_aha1542c_bios_534201-00.bin
SCSI | <b>BusLogic BT-545S</b> | 16-bit SCSI controller. | BusLogic_BT-545S_U15_27128_5002026-4.50.bin

<hr>

## Misc Cards

Hardware | Note
--- | ---
Serial mouse | A Microsoft compatible serial mouse on COM1. Compatible drivers are all over the place for this.
M24 mouse | I haven't seen a DOS mouse driver for this yet but the regular scancode mode works as does the Windows 1.x driver.
PC1512 mouse | The PC1512's perculiar quadrature mouse. You need Amstrad's actual driver for this one.
PS/2 mouse | A standard 2 button PS/2 mouse. As with serial, compatible drivers are common.
Microsoft PS/2 Intellimouse | A PS/2 mouse with mouse wheel.
ATAPI CD-ROM | Works with OAKCDROM.SYS, VDD-IDE.SYS, and the internal drivers of every OS I've tried.
