#!/bin/bash

ARCH="linux-amd64"

BUILD_DIR="build-$ARCH"

cmake -B $BUILD_DIR . \
&& cmake --build $BUILD_DIR --parallel $(nproc)

if [ $? -ne 0 ]; then
    echo >&2 "$BUILD_DIR failed."
    exit 1
fi

source "$BUILD_DIR/docker/version.sh"

OUTPUT_FILE="$OUTPUT_DIR/$PROJECT_NAME-v$PROJECT_VERSION_MAJOR.$PROJECT_VERSION_MINOR-$ARCH"

mkdir -p $OUTPUT_DIR
cp -v "$BUILD_DIR/$PROJECT_NAME" "$OUTPUT_FILE"

echo "::set-output name=$ARCH::${OUTPUT_FILE#/}"