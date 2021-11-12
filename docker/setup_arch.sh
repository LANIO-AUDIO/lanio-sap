#!/bin/bash

pacman --noconfirm -Sy gcc make cmake git && \

git clone https://aur.archlinux.org/pikaur.git /tmp/pikaur && \
(cd /tmp/pikaur && makepkg -fsi --noconfirm) && \

pikaur -S arm-linux-gnueabihf-gcc75-linaro-bin mingw-w64-cmake