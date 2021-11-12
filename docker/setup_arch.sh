#!/bin/bash

pacman --noconfirm -Sy gcc make cmake git fakeroot && \

useradd builduser -G wheel -m && \
su - builduser -c "git clone https://aur.archlinux.org/pikaur.git /tmp/pikaur" && \
su - builduser -c "cd /tmp/pikaur && makepkg -f" && \
pacman --noconfirm -U /tmp/pikaur/pikaur-*.pkg.tar.zst

su - builduser -c "pikaur -S arm-linux-gnueabihf-gcc75-linaro-bin mingw-w64-cmake"