#!/bin/bash

set -x

if [ $# -eq 0 ]
then
    n=2
else
    n=$(($1 - 1))
fi

for nn in $(seq 0 "$n")
do
    # workaround; for some reason, killall gst-launch-1.0 && screen -X -S cameras quit
    # says that a screen session can't be found even though it exists
    ssh -A -t rpib.local ssh -A -t "jn${nn}.local" "killall gst-launch-1.0"
    ssh -A -t rpib.local ssh -A -t "jn${nn}.local" "screen -X -S cameras quit"
done
