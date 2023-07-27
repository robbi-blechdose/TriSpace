# TriSpace

An elite-like space game for the FunKey S.
Includes procedural generation of star systems, space combat, ship upgrades, contracts and more.

## Controls
D-Pad: Ship controls (up/down/roll left/roll right)
A: Fire weapons
B: Fire missile
X/Y: Accelerate/Decelerate ship
Start: Switch to starmap
Select: Activate autodocking
L/R: Station: Switch tabs

## Building

Required libraries (debian package names):
```
libsdl1.2-dev
libsdl-image1.2-dev
libsdl-mixer1.2-dev
libmikmod-dev
```

KeyCraft uses [TinyGL](https://github.com/C-Chads/tinygl) for rendering.  

Since the FunKey uses 16-bit color, adjusting `include/zfeatures.h` is required:
```
#define TGL_NO_DRAW_COLOR 0xf81f

#define TGL_FEATURE_16_BITS        1
#define TGL_FEATURE_32_BITS        0
```

Compile TinyGL via
```
gcc -O3 -flto -c *.c
ar rcs libTinyGL.a *.o
```
for linux and via
```
/opt/funkey-sdk/usr/bin/arm-linux-gcc -O3 -flto -c *.c -march=armv7-a+neon-vfpv4 -mtune=cortex-a7 -mfpu=neon-vfpv4
/opt/funkey-sdk/usr/bin/arm-linux-gcc-ar rcs libTinyGL-fk.a *.o
```
for the FunKey.

Then adjust the `TINGYGL_LIB` and `TINYGL_INCLUDE` paths in the makefile and funkey.mk.

Finally, you can compile the project with `make`.  
`make debug` creates a debug build for linux.  
`make TARGET=funkey` creates a FunKey build.  
`make oclean` cleans up object files. Required before switching between linux and FunKey builds.  
`./package.sh` combines several commands to create a ready-to-go OPK file for the FunKey.