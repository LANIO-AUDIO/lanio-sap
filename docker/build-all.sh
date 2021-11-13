#!/bin/bash

BDIR_LINUX_AMD64="build-linux-amd64"
BDIR_LINUX_ARMHF="build-linux-armhf"
BDIR_WINDOWS_AMD64="build-windows-amd4"

cmake \
    -B $BDIR_LINUX_ARMHF \
    -DCMAKE_TOOLCHAIN_FILE=armhf-toolchain.cmake \
    -DCMAKE_SYSROOT=$RPI_SYSROOT \
    . \
&& cmake --build $BDIR_LINUX_ARMHF
if [ $? -ne 0 ]; then
    echo >&2 "$BDIR_LINUX_ARMHF failed."
    exit 1
fi

x86_64-w64-mingw32-cmake -B $BDIR_WINDOWS_AMD64 . && cmake --build $BDIR_WINDOWS_AMD64
if [ $? -ne 0 ]; then
    echo >&2 "$BDIR_WINDOWS_AMD64 failed."
    exit 1
fi

cmake -B $BDIR_LINUX_AMD64 . && cmake --build $BDIR_LINUX_AMD64
if [ $? -ne 0 ]; then
    echo >&2 "$BDIR_LINUX_AMD64 failed."
    exit 1
fi
