#!/bin/sh

make oclean
make TARGET=funkey

mkdir -p opk
cp  main-funkey.elf opk/main-funkey.elf
cp -r ./res opk/res
cp icon.png opk/icon.png
cp TriSpace.funkey-s.desktop opk/TriSpace.funkey-s.desktop

mksquashfs ./opk TriSpace.opk -all-root -noappend -no-exports -no-xattrs

rm -r opk