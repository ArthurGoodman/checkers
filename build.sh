#!/bin/sh
set -e
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
if [ -f /.dockerenv ]; then
    BUILD_DIR=$DIR/build
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    if [ ! -f $BUILD_DIR/CMakeCache.txt ]; then
        qmake $DIR/cpp
    fi
    make
else
    $DIR/buildenv/run-docker.sh $DIR/build.sh
fi
