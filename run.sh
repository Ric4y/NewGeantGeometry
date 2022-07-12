#!/bin/bash

folder="$(pwd)/LCSim"
if [ $# -eq 0 ]
then
    if [ -d "$folder" ]
    then
        ls -lha $folder
    else
        mkdir "$folder"
    fi
else
    folder="$@"
fi

xhost +
docker run -v $folder:/opt/run:z -w /opt -it -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:z jcfp/optsim
