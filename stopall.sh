#!/bin/bash

set -x

if [ $# -eq 0 ]
then
    n=3
else
    n=$(($1 - 1))
fi

for nn in $(seq 0 "$n")
do
    ssh -J rpib.local "jn${nn}.local" "/home/john/livefeed/stopcapture.sh"
done
