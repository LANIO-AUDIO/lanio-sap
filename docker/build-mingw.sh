#!/bin/bash

BDIR_WINDOWS_AMD64="build-windows-amd4"

x86_64-w64-mingw32-cmake -B $BDIR_WINDOWS_AMD64 . && cmake --build $BDIR_WINDOWS_AMD64
if [ $? -ne 0 ]; then
    echo >&2 "$BDIR_WINDOWS_AMD64 failed."
    exit 1
fi