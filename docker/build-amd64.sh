#!/bin/bash

BDIR_LINUX_AMD64="build-linux-amd64"

cmake -B $BDIR_LINUX_AMD64 . \
&& cmake --build $BDIR_LINUX_AMD64 --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo >&2 "$BDIR_LINUX_AMD64 failed."
    exit 1
fi