#!/bin/bash

BDIR_WINDOWS_AMD64="build-windows-amd4"

x86_64-w64-mingw32-cmake -B $BDIR_WINDOWS_AMD64 . \
&& cmake --build $BDIR_WINDOWS_AMD64 --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo >&2 "$BDIR_WINDOWS_AMD64 failed."
    exit 1
fi

source "$BDIR_WINDOWS_AMD64/docker/version.sh"

cp -v "$BDIR_WINDOWS_AMD64/$PROJECT_NAME.exe" "/output/$PROJECT_NAME.exe"