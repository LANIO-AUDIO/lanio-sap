#!/bin/bash

BDIR_LINUX_AMD64="build-linux-amd64"

cmake -B $BDIR_LINUX_AMD64 . \
&& cmake --build $BDIR_LINUX_AMD64 --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo >&2 "$BDIR_LINUX_AMD64 failed."
    exit 1
fi

source "$BDIR_LINUX_AMD64/docker/version.sh"

cp -v "$BDIR_LINUX_AMD64/$PROJECT_NAME" "/output/$PROJECT_NAME-linux-amd64"