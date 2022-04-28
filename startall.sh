#!/bin/bash

if [ $# -eq 0 ]
then
    n=2
else
    n=$(($1 - 1))
fi

for nn in $(seq 0 "$n")
do
    ssh -A -t rpib.local ssh -A -t "jn${nn}.local" "livefeed/startcapture.sh"
done
