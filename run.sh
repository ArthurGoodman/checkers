#!/bin/sh
set -e
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
if [ -f /.dockerenv ]; then
    node index.js
else
    $DIR/buildenv/run-docker.sh $DIR/run.sh
fi
