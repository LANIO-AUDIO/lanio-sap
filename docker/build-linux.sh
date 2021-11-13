#!/bin/bash

SCRIPT_DIR="$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

    $SCRIPT_DIR/build-armhf.sh \
 && $SCRIPT_DIR/build-amd64.sh \
 && exit 0 \
 || exit 1