#!/bin/bash

for n in jn0.local jn1.local jn2.local
do
    ssh "$n" "killall gst-launch-1.0 && killall screen"
done
