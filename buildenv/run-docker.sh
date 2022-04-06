#!/bin/sh
set -e
DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd -P)
PROJECTDIR=$(realpath $DIR/..)
DOCKERHOME=$PROJECTDIR/build/home
mkdir -p $DOCKERHOME
. $DIR/image-info.sh
if [ -z "$(docker images -q $IMAGE_TAG 2> /dev/null)" ]; then
    $DIR/build-docker.sh
fi
docker run \
    -ti \
    --rm \
    -h $(hostname) \
    --device=/dev/dri/card0:/dev/dri/card0 \
    -v /etc/passwd:/etc/passwd:ro \
    -v /etc/group:/etc/group:ro \
    -v /etc/shadow:/etc/shadow:ro \
    -v $HOME/.Xauthority:$HOME/.Xauthority \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -e DISPLAY \
    -e TERM \
    -u $(id -u):$(id -g) \
    -w $PROJECTDIR \
    -v $DOCKERHOME:$HOME \
    -v $PROJECTDIR:$PROJECTDIR \
    -p 80:80 \
    -p 443:443 \
    --privileged \
    $IMAGE_TAG "$@"
