#!/bin/bash

screen -fn -d -m -S cameras -t video0

#screen -S cameras -p 0 -X stuff "gst-launch-1.0 v4l2src device=/dev/video0 ! nvvidconv ! 'video/x-raw(memory:NVMM),format=I420,framerate=(fraction)30/1,width=1920,height=1080' ! nvoverlaysink display-id=0\n"
screen -S cameras -p 0 -X stuff "gst-launch-1.0 v4l2src device=/dev/video0 io-mode=2 ! 'image/jpeg,framerate=(fraction)30/1,width=1920,height=1080'  ! nvjpegdec ! 'video/x-raw' ! nvvidconv ! 'video/x-raw(memory:NVMM)' ! nvoverlaysink display-id=0 sync=false -ev\n"

screen -S cameras -X screen -t video1 1
#screen -S cameras -p 1 -X stuff "gst-launch-1.0 v4l2src device=/dev/video1 ! nvvidconv ! 'video/x-raw(memory:NVMM),format=I420,framerate=(fraction)30/1,width=1920,height=1080' ! nvoverlaysink display-id=1\n"
screen -S cameras -p 1 -X stuff "gst-launch-1.0 v4l2src device=/dev/video1 io-mode=2 ! 'image/jpeg,framerate=(fraction)30/1,width=1920,height=1080'  ! nvjpegdec ! 'video/x-raw' ! nvvidconv ! 'video/x-raw(memory:NVMM)' ! nvoverlaysink display-id=1 sync=false -ev\n"

screen -S cameras -X screen -t "" 2
