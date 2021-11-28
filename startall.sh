#!/bin/bash

for n in jn0.local jn1.local jn2.local
do
    ssh "$n" livefeed/startcapture.sh
done
