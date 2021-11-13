#!/bin/bash

BDIR_LINUX_ARMHF="build-linux-armhf"

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